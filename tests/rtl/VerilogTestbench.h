#ifndef __VERILOGTESTBENCH_H__
#define __VERILOGTESTBENCH_H__

template <typename T>
class VerilogTestbench {
public:
    VerilogTestbench(T *dut);
    void reset();
    void cycle();
    T *dut;
};

template <typename T>
VerilogTestbench<T>::VerilogTestbench(T *dut)
    : dut(dut)
{
    reset();
}

template <typename T>
void VerilogTestbench<T>::reset()
{
    dut->reset = 1;
    cycle();
    dut->reset = 0;
}

template <typename T>
void VerilogTestbench<T>::cycle()
{
    /* High+Low clock edges. */
    for (int i = 0; i < 2; ++i) {
        dut->eval();
        dut->clk = ~dut->clk;
    }
}

#endif // __VERILOGTESTBENCH_H__
