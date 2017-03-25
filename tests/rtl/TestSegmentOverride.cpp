#include <gtest/gtest.h>
#include <VSegmentOverride.h>

#include "VerilogTestbench.h"

class SegmentOverrideTestFixture : public VerilogTestbench<VSegmentOverride>,
    public ::testing::Test {
public:
    void add_override(int segnum)
    {
        after_n_cycles(0, [&]{
            this->dut.microcode_sr_rd_sel = segnum;
            this->dut.segment_override = 1;
            after_n_cycles(1, [&]{
                this->dut.microcode_sr_rd_sel = 0;
                this->dut.segment_override = 0;
            });
        });
    }

    void next_instruction()
    {
        after_n_cycles(0, [&]{
            this->dut.next_instruction = 1;
            after_n_cycles(1, [&]{
                this->dut.next_instruction = 0;
            });
        });
    }
};

// How to select a segment:
//
// - If the microcode forces a segment, use it.
// - If not, and there is a segment override, use that.
// - If not forced and not overriden, then use the default that the microcode
// specifies, unless BP is used as a base register, in which case use SS.

TEST_F(SegmentOverrideTestFixture, UseSSIfBPIsBase)
{
    this->dut.bp_is_base = 1;
    this->dut.microcode_sr_rd_sel = 3;
    cycle();
    ASSERT_EQ(this->dut.sr_rd_sel, 2);
}

TEST_F(SegmentOverrideTestFixture, UseMicrocodeIfNotBPIsBase)
{
    this->dut.bp_is_base = 0;
    this->dut.microcode_sr_rd_sel = 3;
    cycle();
    ASSERT_EQ(this->dut.sr_rd_sel, 3);
}

TEST_F(SegmentOverrideTestFixture, Override)
{
    add_override(0);
    this->dut.bp_is_base = 0;
    this->dut.microcode_sr_rd_sel = 3;
    cycle();
    ASSERT_EQ(this->dut.sr_rd_sel, 0);
}

TEST_F(SegmentOverrideTestFixture, NextInstructionClearsOverride)
{
    add_override(0);
    this->dut.microcode_sr_rd_sel = 3;
    cycle();

    this->dut.microcode_sr_rd_sel = 2;
    next_instruction();
    cycle();

    ASSERT_EQ(this->dut.sr_rd_sel, 2);
}

TEST_F(SegmentOverrideTestFixture, ForceWins)
{
    add_override(0);
    cycle();

    this->dut.force_segment = 1;
    this->dut.bp_is_base = 1;
    this->dut.microcode_sr_rd_sel = 1;

    cycle();

    this->dut.microcode_sr_rd_sel = 1;
    this->dut.eval();

    ASSERT_EQ(this->dut.sr_rd_sel, 1);
}
