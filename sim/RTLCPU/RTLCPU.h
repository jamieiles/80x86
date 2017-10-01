#pragma once

#include <functional>
#include <map>
#include <string>

#include <VerilogDriver.h>
#include <VRTLCPU.h>

#include "CPU.h"
#include "RegisterFile.h"

static inline void null_io(unsigned long __unused v)
{
}

template <bool debug_enabled = verilator_debug_enabled>
class RTLCPU : public VerilogDriver<VRTLCPU, debug_enabled>, public SimCPU
{
public:
    RTLCPU(const std::string &test_name);
    void write_coverage();
    void reset();
    void write_reg(GPR regnum, uint16_t val);
    uint16_t read_reg(GPR regnum) const;
    void start_instruction();
    void cycle_cpu()
    {
        this->cycle();
    }
    size_t step_with_io(std::function<void(unsigned long)> io_callback);
    void cycle_cpu_with_io(std::function<void(unsigned long)> io_callback);
    void complete_instruction();
    bool int_yield_ready();
    void debug_detach();
    size_t step();
    void idle(int count);
    int time_step();
    void write_flags(uint16_t val);
    uint16_t read_flags() const;
    bool has_trapped();

    int debug_run_proc(
        unsigned addr,
        std::function<void(unsigned long)> io_callback = null_io);
    void debug_seize();
    int debug_step(std::function<void(unsigned long)> io_callback);
    bool debug_is_stopped() const
    {
        return is_stopped;
    }

    bool instruction_had_side_effects() const
    {
        return mem.has_written();
    }

    void clear_side_effects()
    {
        mem.clear_has_written();
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
    void add_ioport(IOPorts *p)
    {
        for (size_t m = 0; m < p->get_num_ports(); ++m)
            io[p->get_base() + m * sizeof(uint16_t)] = p;
    }

    void write_vector8(uint16_t segment,
                       uint16_t addr,
                       const std::vector<uint8_t> &v);
    void write_vector16(uint16_t segment,
                        uint16_t addr,
                        const std::vector<uint16_t> &v);

    virtual bool has_instruction_length() const
    {
        return true;
    }

    virtual void raise_nmi()
    {
        this->after_n_cycles(0, [this] {
            this->dut.nmi = 1;
            this->after_n_cycles(1, [this] { this->dut.nmi = 0; });
        });
        this->cycle();
    }

    virtual void raise_irq(int irq_num)
    {
        assert(irq_num >= 0 && irq_num < 8);

        pending_irqs |= (1 << irq_num);
    }

    unsigned long cycle_count() const
    {
        return static_cast<unsigned long>(this->cur_cycle());
    }

private:
    uint16_t get_microcode_address();
    void mem_access();
    void io_access();
    uint16_t read_ip() const;
    uint16_t read_sr(GPR regnum) const;
    uint16_t read_gpr(GPR regnum) const;
    void write_ip(uint16_t v);
    void write_sr(GPR regnum, uint16_t v);
    void write_gpr(GPR regnum, uint16_t v);
    size_t get_and_clear_instr_length();
    void debug_write_data(uint16_t v);
    void write_mar(uint16_t v);
    void write_mdr(uint16_t v);

    bool mem_in_progress;
    bool io_in_progress;
    int mem_latency;
    std::string test_name;
    bool is_stopped;
    std::map<uint16_t, IOPorts *> io;
    uint8_t pending_irqs;
    bool int_in_progress;
};
