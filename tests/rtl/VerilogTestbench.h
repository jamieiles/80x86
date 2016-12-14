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

template <typename T>
class VerilogTestbench {
public:
    VerilogTestbench(T *dut);
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
    T *dut;
private:
    void run_deferred_events();
    void run_edge_events(EdgeType edge_type);
#ifdef DEBUG
    void setup_trace();
    void teardown_trace();
    VerilatedVcdC tracer;
#endif
    vluint64_t cur_time;
    std::map<vluint64_t, std::vector<std::function<void()>>> deferred_events;
    std::map<EdgeType, vector<std::function<void()>>> edge_events;
};

template <typename T>
VerilogTestbench<T>::VerilogTestbench(T *dut)
    : dut(dut)
{
    dut->reset = 0;
    dut->clk = 0;
    cur_time = 0;
#ifdef DEBUG
    setup_trace();
#endif
    reset();
}

#ifdef DEBUG
template <typename T>
void VerilogTestbench<T>::setup_trace()
{
    Verilated::traceEverOn(true);
    dut->trace(&tracer, 99);

    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    tracer.open((boost::format("%s.%s.vcd") % test_info->test_case_name() %
                 test_info->name()).str().c_str());
}

template <typename T>
void VerilogTestbench<T>::teardown_trace()
{
    tracer.close();
}
#endif

template <typename T>
VerilogTestbench<T>::~VerilogTestbench()
{
#ifdef DEBUG
    teardown_trace();
#endif
    dut->final();
}

template <typename T>
void VerilogTestbench<T>::reset()
{
    dut->reset = 1;
    cycle();
    dut->reset = 0;
}

template <typename T>
void VerilogTestbench<T>::step()
{
    dut->eval();
    dut->clk = !dut->clk;
#ifdef DEBUG
    tracer.dump(cur_time);
#endif
    cur_time++;
}

template <typename T>
void VerilogTestbench<T>::cycle(int count)
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

template <typename T>
void VerilogTestbench<T>::run_edge_events(EdgeType edge_type)
{
    for (auto &e: edge_events[edge_type])
        e();
}

template <typename T>
void VerilogTestbench<T>::run_deferred_events()
{
    auto cycle = cur_time / 2;

    for (auto &e: deferred_events[cycle])
        e();
}

template <typename T>
void VerilogTestbench<T>::at_cycle(vluint64_t cycle_num,
                                   std::function<void()> cb)
{
    assert(cycle_num >= cur_time / 2);

    deferred_events[cycle_num].push_back(cb);
}

#endif // __VERILOGTESTBENCH_H__
