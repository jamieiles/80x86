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

#include <gtest/gtest.h>
#include <VMicrocode.h>
#include <deque>

#include "VerilogTestbench.h"
#include "svdpi.h"

class MicrocodeTestbench : public VerilogTestbench<VMicrocode>,
                           public ::testing::Test
{
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
    bool underflowed;

private:
    std::deque<uint8_t> stream;
};

MicrocodeTestbench::MicrocodeTestbench() : underflowed(false), stream({})
{
    dut.fifo_empty = stream.size() == 0;
    dut.stall = 0;

    svSetScope(svGetScopeFromName("TOP.Microcode"));

    periodic(ClockSetup, [&] {
        this->dut.fifo_empty = this->stream.size() == 0;
        this->dut.fifo_rd_data = this->stream.size() > 0 ? this->stream[0] : 0;
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() > 0) {
            after_n_cycles(0, [&] { this->stream.pop_front(); });
        }
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() == 0)
            underflowed = true;
    });

    reset();
}

TEST_F(MicrocodeTestbench, JumpOpcode)
{
    reset();
    while (current_address() != 0x100)
        cycle();

    set_instruction({0x88});

    ASSERT_EQ(current_address(), 0x100);

    for (int i = 0; i < 16; ++i) {
        if (underflowed)
            FAIL() << "fifo underflow" << std::endl;
        if (current_address() == 0x88)
            return;
        cycle();
    }

    FAIL() << "Didn't hit 0x88 opcode" << std::endl;
}

TEST_F(MicrocodeTestbench, Stall)
{
    // Wait for reset to complete
    for (int i = 0; i < 128; ++i)
        cycle();

    for (int i = 0; i < 32; ++i) {
        cycle();
        EXPECT_EQ(0x100, current_address());
    }

    ASSERT_FALSE(underflowed);
}

TEST_F(MicrocodeTestbench, ExternalStall)
{
    reset();
    while (current_address() != 0x100)
        cycle();

    set_instruction({0x88});

    while (current_address() != 0x88)
        cycle();

    this->dut.stall = 1;
    cycle();

    auto held_address = current_address();
    ASSERT_NE(held_address, 0x88);

    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(current_address(), held_address);
        cycle();
    }

    this->dut.stall = 0;
    cycle(2);
    ASSERT_NE(current_address(), held_address);
}

TEST_F(MicrocodeTestbench, MovUnlocked)
{
    set_instruction({0x88});

    while (current_address() != 0x88)
        cycle();

    while (current_address() != 0x100) {
        ASSERT_FALSE(this->dut.lock);
        cycle();
    }
}

TEST_F(MicrocodeTestbench, LockPrefixMovIsLocked)
{
    set_instruction({0xf0, 0x88});

    while (current_address() != 0x88)
        cycle();

    while (current_address() != 0x100) {
        ASSERT_TRUE(this->dut.lock);
        cycle();
    }
}
