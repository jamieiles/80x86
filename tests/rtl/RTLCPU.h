#pragma once

#include <VCore.h>
#include <VerilogDriver.h>

#include "CPU.h"
#include "RegisterFile.h"

template <bool debug_enabled=verilator_debug_enabled>
class RTLCPU : public VerilogDriver<VCore, debug_enabled>,
    public CPU {
public:
    RTLCPU();
    void reset();
    void write_reg(GPR regnum, uint16_t val);
    uint16_t read_reg(GPR regnum);
    size_t step();
    void write_flags(uint16_t val);
    uint16_t read_flags();
    bool has_trapped() const;
private:
    void data_access();
    void instruction_access();

    bool i_in_progress;
    bool d_in_progress;
    int mem_latency;
};
