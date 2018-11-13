// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#include <stdexcept>
#include <fstream>
#include <VerilogDriver.h>
#include <VRTLCPU.h>
#include <boost/algorithm/string/replace.hpp>

#include "svdpi.h"

#include "Memory.h"
#include "RegisterFile.h"
#include "CPU.h"
#include "RTLCPU.h"

union reg_converter {
    uint16_t v16;
    uint8_t v8[2];
};

static const int max_cycles_per_step = 100000000;

double cur_time_stamp = 0;
double sc_time_stamp()
{
    return cur_time_stamp;
}

template <bool debug_enabled>
RTLCPU<debug_enabled>::RTLCPU(const std::string &test_name)
    : VerilogDriver<VRTLCPU, debug_enabled>(test_name),
      SimCPU(test_name),
      mem_in_progress(false),
      io_in_progress(false),
      mem_latency(0),
      test_name(test_name),
      is_stopped(true)
{
    core_scope = svGetScopeFromName("TOP.RTLCPU.Core");
    microcode_scope = svGetScopeFromName("TOP.RTLCPU.Core.Microcode");

    this->dut.debug_seize = 1;
    this->dut.cache_enabled = 0;
    this->reset();

    this->periodic(ClockSetup, [&] { this->mem_access(); });
    this->periodic(ClockSetup, [&] { this->io_access(); });
    this->periodic(ClockCapture, [&] {
        auto dev = &this->mem;
        if (this->dut.q_m_access && !this->dut.d_io && this->dut.q_m_wr_en) {
            if (this->dut.q_m_bytesel & (1 << 0))
                dev->template write<uint8_t>(this->dut.q_m_addr << 1,
                                             this->dut.q_m_data_out & 0xff);
            if (this->dut.q_m_bytesel & (1 << 1))
                dev->template write<uint8_t>(
                    (this->dut.q_m_addr << 1) + 1,
                    (this->dut.q_m_data_out >> 8) & 0xff);
        }
    });
    this->periodic(ClockCapture, [&] {
        if (this->dut.io_m_access && this->dut.d_io && this->dut.io_m_wr_en &&
            !io_in_progress) {
            if (!this->io.count(this->dut.io_m_addr << 1))
                return;
            auto addr = this->dut.io_m_addr << 1;
            auto p = this->io[this->dut.io_m_addr << 1];
            if (this->dut.io_m_bytesel == 0x3)
                p->write16(addr - p->get_base(), this->dut.io_m_data_out);
            else if (this->dut.io_m_bytesel & (1 << 0))
                p->write8(addr - p->get_base(), 0, this->dut.io_m_data_out);
            else if (this->dut.io_m_bytesel & (1 << 1))
                p->write8(addr - p->get_base(), 1,
                          this->dut.io_m_data_out >> 8);
        }
    });
    this->periodic(ClockCapture,
                   [&] { this->is_stopped = this->dut.debug_stopped; });
    this->periodic(ClockSetup, [&] {
        if (this->int_in_progress || !this->pending_irq) {
            this->dut.intr = 0;
            return;
        }

        if (this->dut.inta) {
            auto irq_num = this->pending_irq;
            this->pending_irq = 0;
            ack_int(irq_num);
        } else {
            int irq_num = this->pending_irq;
            this->dut.intr = 1;
            this->dut.irq = irq_num;
        }
    });
    this->periodic(ClockCapture, [&] {
        if (this->dut.inta)
            this->int_in_progress = false;
    });
}

template <bool debug_enabled>
int RTLCPU<debug_enabled>::debug_run_proc(
    unsigned addr,
    std::function<void(unsigned long)> io_callback)
{
    this->after_n_cycles(0, [&] {
        this->dut.debug_addr = addr;
        this->dut.debug_run = 1;
        this->after_n_cycles(1, [&] { this->dut.debug_run = 0; });
    });

    int cycle_count = 0;

    // Start the procedure running.
    while (debug_is_stopped()) {
        this->cycle();
        ++cycle_count;
    }

    // wait for completion
    cycle_count = 0;
    while (!debug_is_stopped()) {
        this->cycle();

        auto addr = get_microcode_address();
        // Dispatch address and debug idle don't count
        if (addr != 0x100 && addr != 0x102)
            ++cycle_count;

        io_callback(this->cur_cycle());
    }

    if (cycle_count >= max_cycles_per_step)
        throw std::runtime_error("execution timeout");

    return cycle_count;
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::enable_cache()
{
    this->dut.cache_enabled = 1;
}

template <bool debug_enabled>
int RTLCPU<debug_enabled>::debug_step(
    std::function<void(unsigned long)> io_callback)
{
    assert(is_stopped);

    return debug_run_proc(0x00, io_callback);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::debug_detach()
{
    this->after_n_cycles(0, [&] {
        this->dut.debug_seize = 0;
        this->dut.debug_addr = 0;
        this->dut.debug_run = 1;
        this->after_n_cycles(1, [&] { this->dut.debug_run = 0; });
    });
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::start_instruction()
{
    assert(is_stopped);

    // Give the FIFO sufficient time to fill so that we go straight to the
    // instruction and can detect the first real yield.
    this->cycle((mem_latency + 1) * 128);

    this->after_n_cycles(0, [&] {
        this->dut.debug_addr = 0;
        this->dut.debug_run = 1;
        this->after_n_cycles(1, [&] { this->dut.debug_run = 0; });
    });

    while (debug_is_stopped())
        this->cycle();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::complete_instruction()
{
    while (!debug_is_stopped())
        this->cycle();
}

template <bool debug_enabled>
bool RTLCPU<debug_enabled>::int_yield_ready()
{
    svSetScope(microcode_scope);

    return this->dut.get_ext_int_yield();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_coverage()
{
#ifdef COVERAGE
    auto filename = (boost::format("microcode_%s.mcov") % test_name).str();
    boost::replace_all(filename, "/", "_");
    std::ofstream cov;
    cov.open("coverage/" + filename);

    svSetScope(microcode_scope);
    auto num_bins = this->dut.get_microcode_num_instructions();
    for (auto i = 0; i < num_bins; ++i) {
        auto count = this->dut.get_microcode_coverage_bin(i);
        cov << std::hex << i << ":" << count << std::endl;
    }
    cov.close();
#endif
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::reset()
{
    this->dut.nmi = 0;
    this->dut.intr = 0;

    pending_irq = 0;
    int_in_progress = false;

    VerilogDriver<VRTLCPU, debug_enabled>::reset();

    while (get_microcode_address() != 0x102)
        this->cycle();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_reg(GPR regnum, uint16_t val)
{
    if (regnum == IP)
        write_ip(val);
    else if (regnum >= ES && regnum <= DS)
        write_sr(regnum, val);
    else
        write_gpr(regnum, val);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_sr(GPR regnum, uint16_t val)
{
    debug_write_data(val);
    debug_run_proc(0x13 + static_cast<int>(regnum));
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::debug_write_data(uint16_t val)
{
    this->after_n_cycles(0, [&] {
        this->dut.debug_wr_en = 1;
        this->dut.debug_wr_val = val;
        this->after_n_cycles(1, [&] { this->dut.debug_wr_en = 0; });
    });
    this->cycle();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_ip(uint16_t val)
{
    assert(is_stopped);

    debug_write_data(val);
    debug_run_proc(0x11);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_gpr(GPR regnum, uint16_t val)
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

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_reg(GPR regnum) const
{
    if (regnum == IP)
        return read_ip();
    else if (regnum >= ES && regnum <= DS)
        return read_sr(regnum);
    else
        return read_gpr(regnum);
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_ip() const
{
    assert(this->dut.debug_stopped);
    const_cast<RTLCPU<debug_enabled> *>(this)->debug_run_proc(0x0f);

    return this->dut.debug_val;
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_gpr(GPR regnum) const
{
    if (regnum < NUM_16BIT_REGS) {
        const_cast<RTLCPU<debug_enabled> *>(this)->debug_run_proc(
            0x03 + static_cast<int>(regnum));

        return this->dut.debug_val;
    }

    auto regsel = static_cast<int>(regnum - AL) & 0x3;
    const_cast<RTLCPU<debug_enabled> *>(this)->debug_run_proc(
        0x03 + static_cast<int>(regsel));

    reg_converter conv;
    conv.v16 = this->dut.debug_val;

    return conv.v8[regnum >= AH];
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_sr(GPR regnum) const
{
    const_cast<RTLCPU<debug_enabled> *>(this)->debug_run_proc(
        0x03 + static_cast<int>(regnum));

    return this->dut.debug_val;
}

template <bool debug_enabled>
size_t RTLCPU<debug_enabled>::get_and_clear_instr_length()
{
    svSetScope(core_scope);

    return static_cast<size_t>(this->dut.get_and_clear_instr_length());
}

template <bool debug_enabled>
size_t RTLCPU<debug_enabled>::step()
{
    return step_with_io(null_io);
}

template <bool debug_enabled>
size_t RTLCPU<debug_enabled>::step_with_io(
    std::function<void(unsigned long)> io_callback)
{
    this->get_and_clear_instr_length();

    this->debug_step(io_callback);

    return this->get_and_clear_instr_length();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::cycle_cpu_with_io(
    std::function<void(unsigned long)> io_callback)
{
    this->cycle();

    io_callback(this->cur_cycle());
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::idle(int count)
{
    this->cycle(count);
}

template <bool debug_enabled>
int RTLCPU<debug_enabled>::time_step()
{
    return this->debug_step(null_io);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_flags(uint16_t val)
{
    debug_write_data(val);
    debug_run_proc(0x12);
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_flags() const
{
    const_cast<RTLCPU<debug_enabled> *>(this)->debug_run_proc(0x10);

    return this->dut.debug_val;
}

template <bool debug_enabled>
bool RTLCPU<debug_enabled>::has_trapped()
{
    auto int_cs = read_mem16(0, VEC_INT + 2);
    auto int_ip = read_mem16(0, VEC_INT + 0);

    return read_sr(CS) == int_cs && read_ip() == int_ip;
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::mem_access()
{
    if (this->dut.reset || !this->dut.q_m_access || this->dut.d_io ||
        mem_in_progress)
        return;

    this->after_n_cycles(0, [&] {
        auto v = this->mem.template read<uint16_t>(this->dut.q_m_addr << 1);
        this->dut.q_m_data_in = v;
    });
    mem_in_progress = true;
    this->after_n_cycles(mem_latency, [&] {
        this->dut.q_m_ack = 1;
        this->after_n_cycles(1, [&] {
            this->dut.q_m_ack = 0;
            mem_in_progress = false;
        });
    });
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::io_access()
{
    if (this->dut.reset || !this->dut.io_m_access || !this->dut.d_io ||
        io_in_progress)
        return;

    this->after_n_cycles(0, [&] {
        if (this->dut.io_m_wr_en || !this->io.count(this->dut.io_m_addr << 1)) {
            this->dut.io_m_data_in = 0;
            return;
        }

        auto addr = this->dut.io_m_addr << 1;
        auto p = this->io[this->dut.io_m_addr << 1];
        if (this->dut.io_m_bytesel == 0x3)
            this->dut.io_m_data_in = p->read16(addr - p->get_base());
        else if (this->dut.io_m_bytesel & 0x1)
            this->dut.io_m_data_in = p->read8(addr - p->get_base(), 0);
        else if (this->dut.io_m_bytesel & 0x2)
            this->dut.io_m_data_in = p->read8(addr - p->get_base(), 1) << 8;
    });
    io_in_progress = true;
    this->after_n_cycles(mem_latency, [&] {
        this->dut.io_m_ack = 1;
        this->after_n_cycles(1, [&] {
            this->dut.io_m_ack = 0;
            io_in_progress = false;
        });
    });
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::get_microcode_address()
{
    svSetScope(microcode_scope);

    return this->dut.get_microcode_address();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_mem8(uint16_t segment,
                                       uint16_t addr,
                                       uint8_t val)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x23); // Write mem 8
    write_reg(DS, prev_ds);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_mem16(uint16_t segment,
                                        uint16_t addr,
                                        uint16_t val)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x24); // Write mem 16
    write_reg(DS, prev_ds);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_vector8(uint16_t segment,
                                          uint16_t addr,
                                          const std::vector<uint8_t> &v)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);

    write_mar(addr);
    for (auto &b : v) {
        write_mdr(b);
        debug_run_proc(0x23); // Write mem 8
    }

    write_reg(DS, prev_ds);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_vector16(uint16_t segment,
                                           uint16_t addr,
                                           const std::vector<uint16_t> &v)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);

    write_mar(addr);
    for (auto &b : v) {
        write_mdr(b);
        debug_run_proc(0x24); // Write mem 16
    }

    write_reg(DS, prev_ds);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_mem32(uint16_t segment,
                                        uint16_t addr,
                                        uint32_t val)
{
    write_mem16(segment, addr, val & 0xffff);
    write_mem16(segment, addr + 2, val >> 16);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_mar(uint16_t v)
{
    debug_write_data(v);
    debug_run_proc(0x1f);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_mdr(uint16_t v)
{
    debug_write_data(v);
    debug_run_proc(0x20);
}

template <bool debug_enabled>
uint8_t RTLCPU<debug_enabled>::read_mem8(uint16_t segment, uint16_t addr)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    debug_run_proc(0x21); // Read mem 8

    uint8_t val = this->dut.debug_val;

    write_reg(DS, prev_ds);

    return val;
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_mem16(uint16_t segment, uint16_t addr)
{
    auto prev_ds = read_reg(DS);

    write_reg(DS, segment);
    write_mar(addr);
    debug_run_proc(0x22); // Read mem 16

    uint16_t val = this->dut.debug_val;

    write_reg(DS, prev_ds);

    return val;
}

template <bool debug_enabled>
uint32_t RTLCPU<debug_enabled>::read_mem32(uint16_t segment, uint16_t addr)
{
    return read_mem16(segment, addr) |
           (static_cast<uint32_t>(read_mem16(segment, addr + 2)) << 16);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_io8(uint32_t addr, uint8_t val)
{
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x27); // Write io 8
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_io16(uint32_t addr, uint16_t val)
{
    write_mar(addr);
    write_mdr(val);
    debug_run_proc(0x28); // Write io 16
}

template <bool debug_enabled>
uint8_t RTLCPU<debug_enabled>::read_io8(uint32_t addr)
{
    write_mar(addr);
    debug_run_proc(0x25); // Read io 8

    return this->dut.debug_val;
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_io16(uint32_t addr)
{
    write_mar(addr);
    debug_run_proc(0x26); // Read io 16

    return this->dut.debug_val;
}

template RTLCPU<verilator_debug_enabled>::RTLCPU(const std::string &);
template void RTLCPU<verilator_debug_enabled>::idle(int count);
template int RTLCPU<verilator_debug_enabled>::time_step();
template void RTLCPU<verilator_debug_enabled>::enable_cache();
