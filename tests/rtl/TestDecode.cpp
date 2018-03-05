// Copyright Jamie Iles, 2018
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
#include <gmock/gmock.h>
#include <VDecodeTestbench.h>
#include <deque>

#include "VerilogTestbench.h"
#include <RegisterFile.h>

enum RepPrefix { REP_PREFIX_NONE, REP_PREFIX_E, REP_PREFIX_NE };

class Instruction
{
public:
    Instruction(uint8_t opcode)
        : opcode(opcode),
          mod_rm(0),
          displacement(0),
          length(1),
          lock(0),
          rep_prefix(REP_PREFIX_NONE),
          segment(ES),
          has_segment_override(0),
          immed1(0),
          immed2(0)
    {
        immed_count = 0;
    }

    Instruction(uint8_t opcode,
                uint8_t modrm,
                uint16_t displacement,
                uint8_t length,
                uint8_t lock,
                uint8_t rep_prefix,
                uint8_t segment,
                uint8_t has_segment_override,
                uint16_t immed1,
                uint16_t immed2)
        : opcode(opcode),
          mod_rm(modrm),
          displacement(displacement),
          length(length),
          lock(lock),
          rep_prefix(static_cast<RepPrefix>(rep_prefix)),
          segment(static_cast<GPR>(segment + static_cast<int>(ES))),
          has_segment_override(has_segment_override),
          immed1(immed1),
          immed2(immed2)
    {
    }

    bool operator==(const Instruction &rhs) const
    {
        return opcode == rhs.opcode && mod_rm == rhs.mod_rm &&
               displacement == rhs.displacement && length == rhs.length &&
               lock == rhs.lock && rep_prefix == rhs.rep_prefix &&
               segment == rhs.segment &&
               has_segment_override == rhs.has_segment_override &&
               immed1 == rhs.immed1 && immed2 == rhs.immed2;
    }

    Instruction &override_length(uint8_t len)
    {
        this->length = len;

        return *this;
    }

    Instruction &add_rep_prefix(uint8_t pfx)
    {
        this->rep_prefix = static_cast<RepPrefix>(pfx);
        this->length++;

        return *this;
    }

    Instruction &add_modrm(uint8_t mod_rm)
    {
        this->mod_rm = mod_rm;
        this->length++;

        return *this;
    }

    Instruction &add_modrm_disp8(uint8_t mod_rm, uint8_t displacement = 0)
    {
        this->mod_rm = mod_rm;
        this->displacement = displacement;
        this->length += 2;

        return *this;
    }

    Instruction &add_modrm_disp16(uint8_t mod_rm, uint16_t displacement = 0)
    {
        this->mod_rm = mod_rm;
        this->displacement = displacement;
        this->length += 3;

        return *this;
    }

    Instruction &add_lock()
    {
        this->lock = 1;
        this->length++;

        return *this;
    }

    Instruction &add_segment_override(GPR segment)
    {
        this->segment = segment;
        this->has_segment_override = 1;
        this->length++;

        return *this;
    }

    Instruction &add_immed8(uint16_t v)
    {
        if (immed_count == 0)
            immed1 = v;
        else
            immed2 = v;
        ++immed_count;
        this->length++;

        return *this;
    }

    Instruction &add_immed16(uint16_t v)
    {
        if (immed_count == 0)
            immed1 = v;
        else
            immed2 = v;
        ++immed_count;
        this->length += 2;

        return *this;
    }

private:
    uint8_t opcode;
    uint8_t mod_rm;
    uint16_t displacement;
    uint8_t length;
    uint8_t lock;
    RepPrefix rep_prefix;
    GPR segment;
    uint8_t has_segment_override;
    uint16_t immed1;
    uint16_t immed2;
    int immed_count;
};

class DecodeTestbench : public VerilogTestbench<VDecodeTestbench>,
                        public ::testing::Test
{
public:
    DecodeTestbench();

    void set_instruction(const std::vector<uint8_t> &bytes)
    {
        stream.insert(stream.end(), bytes.begin(), bytes.end());
    }

protected:
    std::vector<Instruction> instructions;

private:
    std::deque<uint8_t> stream;
};

DecodeTestbench::DecodeTestbench() : stream({})
{
    periodic(ClockSetup, [&] {
        after_n_cycles(0, [&] {
            if (!this->dut.reset && this->dut.fifo_rd_en &&
                this->stream.size() > 0) {
                this->stream.pop_front();
            }
            this->dut.fifo_rd_data =
                this->stream.size() > 0 ? this->stream[0] : 0;
            this->dut.fifo_empty = stream.size() == 0;
        });
    });

    periodic(ClockCapture, [&] {
        if (!this->dut.complete || this->dut.stall)
            return;
        this->instructions.push_back(Instruction(
            this->dut.opcode, this->dut.mod_rm, this->dut.displacement,
            this->dut.length, this->dut.lock, this->dut.rep, this->dut.segment,
            this->dut.has_segment_override, this->dut.immed1,
            this->dut.immed2));
    });

    reset();
}

TEST_F(DecodeTestbench, SingleByteOpcodeDecodes)
{
    set_instruction({0x90, 0x3f});

    cycle(128);

    std::vector<Instruction> expected = {Instruction(0x90), Instruction(0x3f)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, WithModRM)
{
    set_instruction({0x10, 0xd8});

    cycle(128);

    std::vector<Instruction> expected = {Instruction(0x10).add_modrm(0xd8)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, WithModRMDisplacement8)
{
    // add ax,[bx+0x02]; nop
    set_instruction({0x03, 0x47, 0x02, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x03).add_modrm_disp8(0x47, 0x02), Instruction(0x90)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, WithModRMDisplacement16)
{
    // add ax,[bx+0xaabb]
    set_instruction({0x03, 0x87, 0xbb, 0xaa});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x03).add_modrm_disp16(0x87, 0xaabb)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, LockPrefix)
{
    // lock nop
    set_instruction({0xf0, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {Instruction(0x90).add_lock()};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, MultiplePrefixes)
{
    // lock lock nop
    set_instruction({0xf0, 0xf0, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x90).add_lock().override_length(3)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, RepPrefix)
{
    // repe nop
    set_instruction({0xf3, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x90).add_rep_prefix(REP_PREFIX_E)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, RepnePrefix)
{
    // repne nop
    set_instruction({0xf2, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x90).add_rep_prefix(REP_PREFIX_NE)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, LastRepPrefixUsed)
{
    // repne repe nop
    set_instruction({0xf2, 0xf3, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x90).add_rep_prefix(REP_PREFIX_E).override_length(3)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, SegmentOverride)
{
    // cs: nop
    set_instruction({0x2e, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x90).add_segment_override(CS)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, LastSegmentOverrideUsed)
{
    // cs: ss: nop
    set_instruction({0x2e, 0x36, 0x90});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0x90).add_segment_override(SS).override_length(3)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, Immediate16)
{
    // mov word [bx+0x1234], 0xaa55
    set_instruction({0xc7, 0x87, 0x34, 0x12, 0x55, 0xaa});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0xc7).add_modrm_disp16(0x87, 0x1234).add_immed16(0xaa55)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, Immediate8)
{
    // mov byte [bx+0x1234], 0xa5
    set_instruction({0xc6, 0x87, 0X34, 0X12, 0xa5});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0xc6).add_modrm_disp16(0x87, 0x1234).add_immed8(0xffa5)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, TwoImmediates)
{
    // enter 0x0102, 16
    set_instruction({0xc8, 0x02, 0x01, 0x10});

    cycle(128);

    std::vector<Instruction> expected = {
        Instruction(0xc8).add_immed16(0x0102).add_immed8(0x10)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}

TEST_F(DecodeTestbench, OutputStall)
{
    // mov word [bx+0x1234], 0xaa55
    set_instruction({0xc7, 0x87, 0x34, 0x12, 0x55, 0xaa});

    after_n_cycles(0, [&] { this->dut.stall = 1; });
    cycle(256);
    ASSERT_EQ(instructions.size(), 0UL);
    after_n_cycles(0, [&] { this->dut.stall = 0; });
    cycle(256);

    std::vector<Instruction> expected = {
        Instruction(0xc7).add_modrm_disp16(0x87, 0x1234).add_immed16(0xaa55)};
    EXPECT_THAT(instructions, ::testing::ContainerEq(expected));
}
