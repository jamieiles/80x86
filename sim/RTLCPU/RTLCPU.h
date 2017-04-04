#pragma once

#include <string>

#include <VerilogDriver.h>
#include <VCore.h>

#include "CPU.h"
#include "RegisterFile.h"

template <bool debug_enabled=verilator_debug_enabled>
class RTLCPU : public VerilogDriver<VCore, debug_enabled>,
    public CPU {
public:
    RTLCPU(const std::string &test_name);
    void write_coverage();
    void reset();
    void write_reg(GPR regnum, uint16_t val);
    uint16_t read_reg(GPR regnum);
    size_t step();
    void write_flags(uint16_t val);
    uint16_t read_flags();
    bool has_trapped();

    void debug_run_proc(unsigned addr);
    void debug_seize();
    void debug_step();
    bool debug_is_stopped() const
    {
        return is_stopped;
    }

    bool instruction_had_side_effects() const
    {
        return mem.has_written() || io.has_written();
    }

    void clear_side_effects()
    {
        mem.clear_has_written();
        io.clear_has_written();
    }

    void write_mem8(uint32_t addr, uint8_t val)
    {
        mem.write<uint8_t>(addr, val);
    }
    void write_mem16(uint32_t addr, uint16_t val)
    {
        mem.write<uint16_t>(addr, val);
    }
    void write_mem32(uint32_t addr, uint32_t val)
    {
        mem.write<uint32_t>(addr, val);
    }

    uint8_t read_mem8(uint32_t addr)
    {
        return mem.read<uint8_t>(addr);
    }
    uint16_t read_mem16(uint32_t addr)
    {
        return mem.read<uint16_t>(addr);
    }
    uint32_t read_mem32(uint32_t addr)
    {
        return mem.read<uint32_t>(addr);
    }

    void write_io8(uint32_t addr, uint8_t val)
    {
        io.write<uint8_t>(addr, val);
    }
    void write_io16(uint32_t addr, uint16_t val)
    {
        io.write<uint16_t>(addr, val);
    }
    void write_io32(uint32_t addr, uint32_t val)
    {
        io.write<uint32_t>(addr, val);
    }

    uint8_t read_io8(uint32_t addr)
    {
        return io.read<uint8_t>(addr);
    }
    uint16_t read_io16(uint32_t addr)
    {
        return io.read<uint16_t>(addr);
    }
    uint32_t read_io32(uint32_t addr)
    {
        return io.read<uint32_t>(addr);
    }
private:
    uint16_t get_microcode_address();
    void data_access();
    void instruction_access();
    uint16_t read_ip();
    uint16_t read_sr(GPR regnum);
    uint16_t read_gpr(GPR regnum);
    void write_ip(uint16_t v);
    void write_sr(GPR regnum, uint16_t v);
    void write_gpr(GPR regnum, uint16_t v);
    size_t get_and_clear_instr_length();
    void debug_write_data(uint16_t v);

    bool i_in_progress;
    bool d_in_progress;
    int mem_latency;
    std::string test_name;
    bool is_stopped;
    Memory mem;
    Memory io;
};
