#pragma once

#include <string>

#include <VCore.h>
#include <VerilogDriver.h>

#include "CPU.h"
#include "RegisterFile.h"

template <bool debug_enabled=verilator_debug_enabled>
class RTLCPU : public VerilogDriver<VCore, debug_enabled>,
    public CPU {
public:
    RTLCPU(const std::string &test_name);
    void reset();
    void write_reg(GPR regnum, uint16_t val);
    uint16_t read_reg(GPR regnum);
    size_t step();
    void write_flags(uint16_t val);
    uint16_t read_flags();
    bool has_trapped() const;
private:
    uint16_t get_microcode_address();
    void data_access();
    void instruction_access();
    uint16_t read_ip() const;
    uint16_t read_sr(GPR regnum) const;
    uint16_t read_gpr(GPR regnum) const;
    void write_ip(uint16_t v);
    void write_sr(GPR regnum, uint16_t v);
    void write_gpr(GPR regnum, uint16_t v);
    size_t get_and_clear_instr_length();

    bool i_in_progress;
    bool d_in_progress;
    int mem_latency;
};
