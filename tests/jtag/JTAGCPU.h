#pragma once

#include <string>

#include "CPU.h"
#include "RegisterFile.h"

class JTAGCPU : public CPU {
public:
    JTAGCPU(const std::string &test_name);
    void reset();
    void write_reg(GPR regnum, uint16_t val);
    uint16_t read_reg(GPR regnum);
    size_t step();
    void write_flags(uint16_t val);
    uint16_t read_flags();
    bool has_trapped();
    uint32_t idcode();

    uint16_t debug_run_proc(unsigned addr);
    void debug_seize();
    void debug_step();
    bool debug_is_stopped() const;

    bool instruction_had_side_effects() const
    {
        return false;
    }

    void clear_side_effects()
    {
    }

    void write_mem8(uint16_t segment, uint16_t addr, uint8_t val);
    void write_mem16(uint16_t segment, uint16_t addr, uint16_t val);
    void write_mem32(uint16_t segment, uint16_t addr, uint32_t val);
    uint8_t read_mem8(uint16_t segment, uint16_t addr);
    uint16_t read_mem16(uint16_t segment, uint16_t addr);
    uint32_t read_mem32(uint16_t segment, uint16_t addr);
    void write_io8(uint32_t addr, uint8_t val);
    void write_io16(uint32_t addr, uint16_t val);
    uint8_t read_io8(uint32_t addr);
    uint16_t read_io16(uint32_t addr);
private:
    uint16_t read_ip();
    uint16_t read_sr(GPR regnum);
    uint16_t read_gpr(GPR regnum);
    void write_ip(uint16_t v);
    void write_sr(GPR regnum, uint16_t v);
    void write_gpr(GPR regnum, uint16_t v);
    void debug_write_data(uint16_t v);
    uint16_t debug_read_data();
    void write_mar(uint16_t v);
    void write_mdr(uint16_t v);
    void write_scr(uint16_t v);
    uint16_t read_scr();
    std::string test_name;
};
