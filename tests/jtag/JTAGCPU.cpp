#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <fstream>
#include "JTAGCPU.h"

extern "C" {
#include <jtag.h>
}

enum StatusControlBits {
    STATUS_CONTROL_RUN = (1 << 0),
    STATUS_CONTROL_RESET = (1 << 1),
    STATUS_CONTROL_WR_EN = (1 << 16),
};

union reg_converter {
    uint16_t v16;
    uint8_t v8[2];
};

using JTAGError = std::runtime_error;

JTAGCPU::JTAGCPU(const std::string &test_name)
    : test_name(test_name)
{
    if (jtag_open_virtual_device(0x00))
        throw JTAGError("Failed to open Virtual JTAG");

    this->reset();

    write_scr(0);
    while (read_scr() & STATUS_CONTROL_RUN)
        continue;
}

void JTAGCPU::write_scr(uint16_t v)
{
    if (jtag_vir(1))
        throw JTAGError("Unable to write VIR");
    if (jtag_vdr(17, STATUS_CONTROL_WR_EN | v, NULL))
        throw JTAGError("Unable to write VIR");
}

uint16_t JTAGCPU::read_scr()
{
    uint32_t v;

    if (jtag_vir(1))
        throw JTAGError("Unable to write VIR");
    if (jtag_vdr(17, 0, &v))
        throw JTAGError("Unable to read VIR");

    return static_cast<uint16_t>(v);
}

uint16_t JTAGCPU::debug_read_data()
{
    uint32_t v;

    if (jtag_vir(2))
        throw JTAGError("Unable to write VIR");

    do {
        if (jtag_vdr(17, 0, &v))
            throw JTAGError("Unable to write VIR");
    } while (!(v & (1 << 16)));

    return static_cast<uint16_t>(v);
}

uint16_t JTAGCPU::debug_run_proc(unsigned addr)
{
    if (jtag_vir(3))
        throw JTAGError("Unable to write VIR");
    if (jtag_vdr(8, addr, NULL))
        throw JTAGError("Unable to write VIR");

    while (read_scr() & STATUS_CONTROL_RUN)
        continue;

    return debug_read_data();
}

void JTAGCPU::debug_step()
{
    debug_run_proc(0x00);
}

void JTAGCPU::reset()
{
    write_scr(STATUS_CONTROL_RESET);
    write_scr(0);
}

void JTAGCPU::write_reg(GPR regnum, uint16_t val)
{
    if (regnum == IP)
        write_ip(val);
    else if (regnum >= ES && regnum <= DS)
        write_sr(regnum, val);
    else
        write_gpr(regnum, val);
}

void JTAGCPU::write_sr(GPR regnum, uint16_t val)
{
    debug_write_data(val);
    debug_run_proc(0x13 + static_cast<int>(regnum));
}

void JTAGCPU::debug_write_data(uint16_t val)
{
    if (jtag_vir(2))
        throw JTAGError("Unable to write VIR");
    if (jtag_vdr(17, (1 << 16) | val, NULL))
        throw JTAGError("Unable to write VIR");
}

void JTAGCPU::write_ip(uint16_t val)
{
    debug_write_data(val);
    debug_run_proc(0x11);
}

void JTAGCPU::write_gpr(GPR regnum, uint16_t val)
{
    if (regnum < NUM_16BIT_REGS) {
        debug_write_data(val);
        debug_run_proc(0x13 + static_cast<int>(regnum));
    } else {
        auto regsel = (regnum - AL) & 0x3;
        reg_converter conv;
        conv.v16 = this->read_reg(static_cast<GPR>(regsel));

        conv.v8[regnum >= AH] = val;

        debug_write_data(conv.v16);
        debug_run_proc(0x13 + static_cast<int>(regsel));
    }
}

uint16_t JTAGCPU::read_reg(GPR regnum)
{
    if (regnum == IP)
        return read_ip();
    else if (regnum >= ES && regnum <= DS)
        return read_sr(regnum);
    else
        return read_gpr(regnum);
}

uint16_t JTAGCPU::read_ip()
{
    return debug_run_proc(0x0f);
}

uint16_t JTAGCPU::read_gpr(GPR regnum)
{
    if (regnum < NUM_16BIT_REGS)
        return debug_run_proc(0x03 + static_cast<int>(regnum));

    auto regsel = static_cast<int>(regnum - AL) & 0x3;
    reg_converter conv;

    conv.v16 = debug_run_proc(0x03 + static_cast<int>(regsel));

    return conv.v8[regnum >= AH];
}

uint16_t JTAGCPU::read_sr(GPR regnum)
{
    return debug_run_proc(0x03 + static_cast<int>(regnum));
}

size_t JTAGCPU::step()
{
    this->debug_step();

    return 0;
}

void JTAGCPU::write_flags(uint16_t val)
{
    debug_write_data(val);
    debug_run_proc(0x12);
}

uint16_t JTAGCPU::read_flags()
{
    return debug_run_proc(0x10);
}

bool JTAGCPU::has_trapped()
{
    auto int_cs = read_mem16(0, VEC_INT + 2);
    auto int_ip = read_mem16(0, VEC_INT + 0);

    return read_sr(CS) == int_cs && read_ip() == int_ip;
}

void JTAGCPU::write_mem8(uint16_t segment, uint16_t addr, uint8_t val)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x23); // Write mem 8
    write_reg(DS, prev_ds);
}

void JTAGCPU::write_mem16(uint16_t segment, uint16_t addr, uint16_t val)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x24); // Write mem 16
    write_reg(DS, prev_ds);
}

void JTAGCPU::write_mem32(uint16_t segment, uint16_t addr, uint32_t val)
{
    write_mem16(segment, addr, val & 0xffff);
    write_mem16(segment, addr + 2, val >> 16);
}

void JTAGCPU::write_mar(uint16_t v)
{
    debug_write_data(v);
    debug_run_proc(0x1f);
}

void JTAGCPU::write_mdr(uint16_t v)
{
    debug_write_data(v);
    debug_run_proc(0x20);
}

uint8_t JTAGCPU::read_mem8(uint16_t segment, uint16_t addr)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    uint8_t val = debug_run_proc(0x21); // Read mem 8

    write_reg(DS, prev_ds);

    return val;
}

uint16_t JTAGCPU::read_mem16(uint16_t segment, uint16_t addr)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    uint16_t val = debug_run_proc(0x22); // Read mem 16

    write_reg(DS, prev_ds);

    return val;
}

uint32_t JTAGCPU::read_mem32(uint16_t segment, uint16_t addr)
{
    return read_mem16(segment, addr) |
        (static_cast<uint32_t>(read_mem16(segment, addr + 2)) << 16);
}

void JTAGCPU::write_io8(uint32_t addr, uint8_t val)
{
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x27); // Write io 8
}

void JTAGCPU::write_io16(uint32_t addr, uint16_t val)
{
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x28); // Write io 16
}

uint8_t JTAGCPU::read_io8(uint32_t addr)
{
    write_mar(addr);
    return debug_run_proc(0x25); // Read io 8
}

uint16_t JTAGCPU::read_io16(uint32_t addr)
{
    write_mar(addr);
    return debug_run_proc(0x26); // Read io 16
}

uint32_t JTAGCPU::idcode()
{
    uint32_t idcode = 0;
    jtag_vir(0);
    jtag_vdr(32, 0, &idcode);

    return idcode;
}
