#ifndef __VERILOGTESTBENCH_H__
#define __VERILOGTESTBENCH_H__

#include <verilated_vcd_c.h>

#include <boost/format.hpp>
#include <functional>
#include <map>
#include <vector>

enum EdgeType {
    PosEdge,
    NegEdge,
};

#ifdef DEBUG
const bool verilator_debug_enabled = true;
#else
const bool verilator_debug_enabled = false;
#endif

template <typename T, bool debug_enabled=verilator_debug_enabled>
class VerilogTestbench {
public:
    VerilogTestbench();
    virtual ~VerilogTestbench();
    void reset();
    void at_cycle(vluint64_t cycle_num,
                  std::function<void()> cb);
    void after_n_cycles(vluint64_t delta,
                        std::function<void()> cb)
    {
        at_cycle((cur_time / 2) + delta, cb);
    }
    void at_edge(EdgeType edge_type, std::function<void()> fn)
    {
        edge_events[edge_type].push_back(fn);
    }
    virtual void step();
    virtual void cycle(int count=1);
    vluint64_t cur_cycle() const
    {
        return cur_time / 2;
    }
    T dut;
private:
    void run_deferred_events();
    void run_edge_events(EdgeType edge_type);
    void setup_trace();
    void teardown_trace();
    VerilatedVcdC tracer;
    vluint64_t cur_time;
    std::map<vluint64_t, std::vector<std::function<void()>>> deferred_events;
    std::map<EdgeType, vector<std::function<void()>>> edge_events;
};

template <typename T, bool debug_enabled>
VerilogTestbench<T, debug_enabled>::VerilogTestbench()
{
    dut.reset = 0;
    dut.clk = 0;
    cur_time = 0;
    if (debug_enabled)
        setup_trace();
    reset();
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
void VerilogTestbench<T, debug_enabled>::setup_trace()
{
    if (debug_enabled) {
        Verilated::traceEverOn(true);
        tracer_impl<T, debug_enabled>::trace_dut(&dut, &tracer);

        auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        tracer.open((boost::format("%s.%s.vcd") % test_info->test_case_name() %
                     test_info->name()).str().c_str());
    }
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::teardown_trace()
{
    if (debug_enabled)
        tracer.close();
}

template <typename T, bool debug_enabled>
VerilogTestbench<T, debug_enabled>::~VerilogTestbench()
{
    if (debug_enabled)
        teardown_trace();
    dut.final();
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::reset()
{
    dut.reset = 1;
    cycle();
    dut.reset = 0;
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::step()
{
    dut.eval();
    dut.clk = !dut.clk;
    if (debug_enabled)
        tracer.dump(cur_time);
    cur_time++;
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::cycle(int count)
{
    for (int i = 0; i < count; ++i) {
        run_deferred_events();
        /* High clock edge. */
        step();
        run_edge_events(PosEdge);
        /* Low clock edge. */
        step();
        run_edge_events(NegEdge);
    }
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::run_edge_events(EdgeType edge_type)
{
    for (auto &e: edge_events[edge_type])
        e();
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::run_deferred_events()
{
    auto cycle = cur_time / 2;

    for (auto &e: deferred_events[cycle])
        e();
}

template <typename T, bool debug_enabled>
void VerilogTestbench<T, debug_enabled>::at_cycle(vluint64_t cycle_num,
                                   std::function<void()> cb)
{
    assert(cycle_num >= cur_time / 2);

    deferred_events[cycle_num].push_back(cb);
}

#endif // __VERILOGTESTBENCH_H__
