#pragma once

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <verilated_cov.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/format.hpp>
#include <boost/type_index.hpp>
#include <functional>
#include <map>
#include <vector>

enum PeriodicEventType {
    // Run at the end of the cycle once the DUT has been clocked and
    // evaluated.  Signals should be stable and ready for capturing by the
    // test driver.
    ClockCapture,
    // Run immediately before the positive clock edge, any changed signals
    // will be delivered on the clock edge and not evaluated before.  Use this
    // when sending data into the DUT that would normally be registered in the
    // real system.
    ClockSetup,
};

const int evals_per_cycle = 10;

extern double sc_time_stamp();
extern double cur_time_stamp;

static_assert(evals_per_cycle % 2 == 0, "evals_per_cycle must be divisible by 2");

#ifdef DEBUG
const bool verilator_debug_enabled = true;
#else
const bool verilator_debug_enabled = false;
#endif

#ifdef VM_COVERAGE
const bool verilator_coverage_enabled = true;
#else
const bool verilator_coverage_enabled = false;
#endif

template <typename T, bool debug_enabled=verilator_debug_enabled>
class VerilogDriver {
public:
    VerilogDriver();
    VerilogDriver(const std::string &instance_name);
    VerilogDriver(const VerilogDriver &rhs) = delete;
    virtual ~VerilogDriver();
    void reset();
    void after_n_cycles(vluint64_t delta,
                        std::function<void()> cb)
    {
        at_cycle(cycle_num + delta, cb);
    }
    void periodic(PeriodicEventType edge_type, std::function<void()> fn)
    {
        periodic_events[edge_type].push_back(fn);
    }
    virtual void cycle(int count=1);
    vluint64_t cur_cycle() const
    {
        return cycle_num;
    }
protected:
    T dut;
private:
    void at_cycle(vluint64_t cycle_num, std::function<void()> cb);
    void run_deferred_events();
    void run_periodic_events(PeriodicEventType edge_type);
    void setup_trace();
    void teardown_trace();
    VerilatedVcdC tracer;
    vluint64_t cur_time;
    vluint64_t cycle_num;
    std::map<vluint64_t, std::vector<std::function<void()>>> deferred_events;
    std::map<PeriodicEventType, vector<std::function<void()>>> periodic_events;
    std::string instance_name;
};
template <typename T, bool debug_enabled>
VerilogDriver<T, debug_enabled>::VerilogDriver()
    : VerilogDriver<T, debug_enabled>(boost::typeindex::type_id<T>().pretty_name())
{
}

template <typename T, bool debug_enabled>
VerilogDriver<T, debug_enabled>::VerilogDriver(const std::string &instance_name)
    : cycle_num(0),
    instance_name(instance_name)
{
    dut.reset = 0;
    dut.clk = 0;
    cur_time = 0;
    cur_time_stamp = 0;
    if (debug_enabled)
        setup_trace();
}

template <typename T, bool debug_enabled>
struct tracer_impl {
};

template <typename T>
struct tracer_impl<T, true> {
    static void trace_dut(T *dut, VerilatedVcdC *tracer)
    {
        dut->trace(tracer, 99);
    }
};

template <typename T>
struct tracer_impl<T, false> {
    static void trace_dut(T *, VerilatedVcdC *)
    {
    }
};

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::setup_trace()
{
    if (debug_enabled) {
        Verilated::traceEverOn(true);
        tracer_impl<T, debug_enabled>::trace_dut(&dut, &tracer);

        auto filename = (boost::format("%s.vcd") % instance_name).str();
        boost::replace_all(filename, "/", "_");
        tracer.open(filename.c_str());
    }
}

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::teardown_trace()
{
    if (debug_enabled)
        tracer.close();
}

template <typename T, bool debug_enabled>
VerilogDriver<T, debug_enabled>::~VerilogDriver()
{
    if (debug_enabled)
        teardown_trace();

    dut.final();

    if (verilator_coverage_enabled) {
        auto filename = (boost::format("%s.dat") % instance_name).str();
        boost::replace_all(filename, "/", "_");
        VerilatedCov::write(("coverage/" + filename).c_str());
        VerilatedCov::clear();
    }
}

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::reset()
{
    this->dut.reset = 1;
    after_n_cycles(2, [&]{
        this->dut.reset = 0;
    });
    do {
        cycle();
    } while (dut.reset);
}

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::cycle(int count)
{
    for (int i = 0; i < count; ++i) {
        for (auto j = 0; j < evals_per_cycle; ++j) {

            if (debug_enabled)
                tracer.dump(cur_time);
            if (j == 0) {
                run_periodic_events(ClockSetup);
                dut.clk = !dut.clk;
                dut.eval();
                run_deferred_events();
                dut.eval();
                run_periodic_events(ClockCapture);
            } else if (j == evals_per_cycle / 2) {
                dut.clk = !dut.clk;
                dut.eval();
            } else {
                dut.eval();
            }
            ++cur_time;
        }

        ++cycle_num;
        ++cur_time_stamp;
    }
}

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::run_periodic_events(PeriodicEventType edge_type)
{
    for (auto &e: periodic_events[edge_type])
        e();
}

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::run_deferred_events()
{
    for (auto &e: deferred_events[cycle_num])
        e();
}

template <typename T, bool debug_enabled>
void VerilogDriver<T, debug_enabled>::at_cycle(vluint64_t target_cycle_num,
                                               std::function<void()> cb)
{
    assert(target_cycle_num >= cycle_num);

    deferred_events[target_cycle_num].push_back(cb);
}
