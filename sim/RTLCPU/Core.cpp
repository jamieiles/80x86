#include <stdexcept>
#include <fstream>
#include <VCore.h>
#include <VerilogDriver.h>
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

static const int max_cycles_per_step = 1000000;

double cur_time_stamp = 0;
double sc_time_stamp()
{
    return cur_time_stamp;
}

template <bool debug_enabled>
RTLCPU<debug_enabled>::RTLCPU(const std::string &test_name)
    : VerilogDriver<VCore, debug_enabled>(test_name),
    i_in_progress(false),
    d_in_progress(false),
    mem_latency(0),
    test_name(test_name)
{
    this->reset();

    this->periodic(ClockSetup, [&]{ this->data_access(); });
    this->periodic(ClockSetup, [&]{ this->instruction_access(); });
    this->periodic(ClockCapture, [&]{
        auto dev = !this->dut.d_io ? &this->mem : &this->io;
        if (this->dut.data_m_access && this->dut.data_m_wr_en) {
            if (this->dut.data_m_bytesel & (1 << 0))
                dev->template write<uint8_t>(this->dut.data_m_addr << 1,
                                             this->dut.data_m_data_out & 0xff);
            if (this->dut.data_m_bytesel & (1 << 1))
                dev->template write<uint8_t>((this->dut.data_m_addr << 1) + 1,
                                             (this->dut.data_m_data_out >> 8) & 0xff);
        }
    });
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_coverage()
{
#ifdef COVERAGE
    auto filename = (boost::format("microcode_%s.mcov") % test_name).str();
    boost::replace_all(filename, "/", "_");
    std::ofstream cov;
    cov.open("coverage/" + filename);

    svSetScope(svGetScopeFromName("TOP.Core.Microcode"));
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
    VerilogDriver<VCore, debug_enabled>::reset();
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
    svSetScope(svGetScopeFromName("TOP.Core.SegmentRegisterFile"));
    this->dut.set_sr(regnum - ES, val);

    if (regnum == CS) {
        svSetScope(svGetScopeFromName("TOP.Core.Prefetch"));
        this->dut.set_cs(val);
    }
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_ip(uint16_t val)
{
    svSetScope(svGetScopeFromName("TOP.Core.IP"));
    this->dut.set_ip(val);

    svSetScope(svGetScopeFromName("TOP.Core.Prefetch"));
    this->dut.set_ip(val);
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_gpr(GPR regnum, uint16_t val)
{
    svSetScope(svGetScopeFromName("TOP.Core.RegisterFile"));

    if (regnum < NUM_16BIT_REGS) {
        this->dut.set_gpr(static_cast<int>(regnum), val);
    } else {
        auto regsel = static_cast<int>(regnum - AL) & 0x3;
        reg_converter conv;
        conv.v16 = this->dut.get_gpr(regsel);

        conv.v8[regnum >= AH] = val;

        this->dut.set_gpr(regsel, conv.v16);
    }
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_reg(GPR regnum)
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
    svSetScope(svGetScopeFromName("TOP.Core.IP"));

    return this->dut.get_ip();
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_gpr(GPR regnum) const
{
    svSetScope(svGetScopeFromName("TOP.Core.RegisterFile"));

    if (regnum < NUM_16BIT_REGS)
        return this->dut.get_gpr(static_cast<int>(regnum));

    auto regsel = static_cast<int>(regnum - AL) & 0x3;
    reg_converter conv;
    conv.v16 = this->dut.get_gpr(regsel);

    return conv.v8[regnum >= AH];
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_sr(GPR regnum) const
{
    svSetScope(svGetScopeFromName("TOP.Core.SegmentRegisterFile"));
    return this->dut.get_sr(regnum - ES);
}

template <bool debug_enabled>
size_t RTLCPU<debug_enabled>::get_and_clear_instr_length()
{
    svSetScope(svGetScopeFromName("TOP.Core"));

    return static_cast<size_t>(this->dut.get_and_clear_instr_length());
}

template <bool debug_enabled>
size_t RTLCPU<debug_enabled>::step()
{
    this->get_and_clear_instr_length();

    // Wait for opcode dispatch
    while (this->get_microcode_address() != 0x100)
        this->cycle();

    // Wait for opcode dispatch to start
    do {
        this->cycle();
    } while (this->get_microcode_address() == 0x100);

    // Return to opcode dispatch
    int cycle_count = 0;
    while (this->get_microcode_address() != 0x100 &&
           cycle_count++ < max_cycles_per_step)
        this->cycle();

    if (cycle_count >= max_cycles_per_step)
        throw std::runtime_error("execution timeout");

    return this->get_and_clear_instr_length();
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::write_flags(uint16_t val)
{
    svSetScope(svGetScopeFromName("TOP.Core.Flags"));
    this->dut.set_flags(val);
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::read_flags()
{
    svSetScope(svGetScopeFromName("TOP.Core.Flags"));
    return this->dut.get_flags();
}

template <bool debug_enabled>
bool RTLCPU<debug_enabled>::has_trapped() const
{
    auto int_cs = this->mem.template read<uint16_t>(VEC_INT + 2);
    auto int_ip = this->mem.template read<uint16_t>(VEC_INT + 0);

    return read_sr(CS) == int_cs && read_ip() == int_ip;
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::instruction_access()
{
    if (this->dut.reset || !this->dut.instr_m_access || i_in_progress)
        return;

    i_in_progress = true;
    this->after_n_cycles(mem_latency, [&]{
        this->dut.instr_m_data_in =
            this->mem.template read<uint16_t>(this->dut.instr_m_addr << 1);
        this->dut.instr_m_ack = 1;
        this->after_n_cycles(1, [&]{
            this->dut.instr_m_ack = 0;
            i_in_progress = false;
        });
    });
}

template <bool debug_enabled>
void RTLCPU<debug_enabled>::data_access()
{
    if (this->dut.reset || !this->dut.data_m_access || d_in_progress)
        return;

    d_in_progress = true;
    this->after_n_cycles(mem_latency, [&]{
        auto v = this->dut.d_io ?
            this->io.template read<uint16_t>(this->dut.data_m_addr << 1) :
            this->mem.template read<uint16_t>(this->dut.data_m_addr << 1);
        this->dut.data_m_data_in = v;
        this->dut.data_m_ack = 1;
        this->after_n_cycles(1, [&]{
            this->dut.data_m_ack = 0;
            d_in_progress = false;
        });
    });
}

template <bool debug_enabled>
uint16_t RTLCPU<debug_enabled>::get_microcode_address()
{
    svSetScope(svGetScopeFromName("TOP.Core.Microcode"));

    return this->dut.get_microcode_address();
}

template RTLCPU<verilator_debug_enabled>::RTLCPU(const std::string &);
