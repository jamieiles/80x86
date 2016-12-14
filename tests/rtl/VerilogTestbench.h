#ifndef __VERILOGTESTBENCH_H__
#define __VERILOGTESTBENCH_H__

#include <verilated_vcd_c.h>
#include <boost/format.hpp>

template <typename T>
class VerilogTestbench {
public:
    VerilogTestbench(T *dut);
    virtual ~VerilogTestbench();
    void reset();
    virtual void step();
    virtual void cycle(int count=1);
    T *dut;
private:
#ifdef DEBUG
    void setup_trace();
    void teardown_trace();
    VerilatedVcdC tracer;
#endif
    vluint64_t cur_time;
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
    /* High+Low clock edges. */
    for (int i = 0; i < count * 2; ++i)
        step();
}

#endif // __VERILOGTESTBENCH_H__
