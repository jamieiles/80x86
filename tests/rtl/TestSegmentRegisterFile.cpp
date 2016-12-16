#include <gtest/gtest.h>
#include <VSegmentRegisterFile.h>

#include "VerilogTestbench.h"

class SegmentRegisterFileTestbench : public VerilogTestbench<VSegmentRegisterFile> {
public:
    SegmentRegisterFileTestbench(VSegmentRegisterFile *dut);
    void write_sr(uint8_t sr, uint16_t v);
    void trigger_read_sr(uint8_t sr);
    uint16_t get_sr() const;
};

SegmentRegisterFileTestbench::SegmentRegisterFileTestbench(VSegmentRegisterFile *dut)
    : VerilogTestbench<VSegmentRegisterFile>(dut)
{
}

void SegmentRegisterFileTestbench::write_sr(uint8_t sr, uint16_t v)
{
    dut->wr_sel = sr;
    dut->wr_val = v;
    dut->wr_en = 1;
    cycle();
    dut->wr_en = 0;
}

void SegmentRegisterFileTestbench::trigger_read_sr(uint8_t sr)
{
    dut->rd_sel = sr;
    cycle();
}

uint16_t SegmentRegisterFileTestbench::get_sr() const
{
    return dut->rd_val;
}

class SegmentRegisterFileTestFixture : public ::testing::Test {
public:
    SegmentRegisterFileTestFixture()
        : tb(&register_file) {}
protected:
    VSegmentRegisterFile register_file;
    SegmentRegisterFileTestbench tb;
};

TEST_F(SegmentRegisterFileTestFixture, segment_read_write)
{
    for (auto r = 0; r < 4; ++r)
        tb.write_sr(r, ~r);

    for (auto r = 0; r < 4; ++r) {
        tb.trigger_read_sr(r);
        EXPECT_EQ(~r & 0xffff, tb.get_sr());
    }
}

TEST_F(SegmentRegisterFileTestFixture, sr_read_during_write)
{
    tb.write_sr(0, 0x1234);
    ASSERT_EQ(0x1234, tb.get_sr());
}

TEST_F(SegmentRegisterFileTestFixture, cs_output_during_write)
{
    tb.write_sr(1, 0xf00f);
    ASSERT_EQ(0xf00f, tb.dut->cs);
}
