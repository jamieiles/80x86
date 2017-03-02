#include <gtest/gtest.h>
#include <VMicrocode.h>
#include <deque>

#include "VerilogTestbench.h"
#include "svdpi.h"

class MicrocodeTestbench : public VerilogTestbench<VMicrocode>,
    public ::testing::Test {
public:
    MicrocodeTestbench();

    void set_instruction(const std::vector<uint8_t> bytes)
    {
        stream.insert(stream.end(), bytes.begin(), bytes.end());
    }

    uint16_t current_address()
    {
        return this->dut.get_microcode_address();
    }
private:
    std::deque<uint8_t> stream;
};

MicrocodeTestbench::MicrocodeTestbench()
{
    svSetScope(svGetScopeFromName("TOP.Microcode"));

    periodic(ClockSetup, [&]{
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() > 0) {
            after_n_cycles(0, [&]{
                this->dut.fifo_empty = this->stream.size() == 0;
                this->dut.fifo_rd_data = this->stream.size() > 0 ?
                    this->stream[0] : 0;
                this->stream.pop_front();
            });
        }
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() == 0)
            FAIL() << "fifo underflow" << std::endl;
    });
}

TEST_F(MicrocodeTestbench, JumpOpcode)
{
    set_instruction({0x30});

    ASSERT_EQ(current_address(), 0x100);

    for (int i = 0; i < 16; ++i) {
        if (current_address() == 0x30)
            return;
        cycle();
    }

    FAIL() << "Didn't hit 0x30 opcode" << std::endl;
}
