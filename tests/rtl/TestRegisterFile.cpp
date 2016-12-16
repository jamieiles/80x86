#include <gtest/gtest.h>
#include <VRegisterFile.h>

#include "VerilogTestbench.h"

class RegisterFileTestbench : public VerilogTestbench<VRegisterFile> {
public:
    RegisterFileTestbench(VRegisterFile *dut);
    void write8(uint8_t reg, uint8_t v);
    void write16(uint8_t reg, uint16_t v);
    void trigger_read8(int port, uint8_t reg);
    void trigger_read16(int port, uint8_t reg);
    uint16_t get_value(int port) const;
};

RegisterFileTestbench::RegisterFileTestbench(VRegisterFile *dut)
    : VerilogTestbench<VRegisterFile>(dut)
{
}

void RegisterFileTestbench::write8(uint8_t reg, uint8_t v)
{
    dut->wr_sel = reg;
    dut->wr_val = v;
    dut->wr_en = 1;
    dut->is_8_bit = 1;
    cycle();
    dut->wr_en = 0;
}

void RegisterFileTestbench::write16(uint8_t reg, uint16_t v)
{
    dut->wr_sel = reg;
    dut->wr_val = v;
    dut->wr_en = 1;
    dut->is_8_bit = 0;
    cycle();
    dut->wr_en = 0;
}

void RegisterFileTestbench::trigger_read8(int port, uint8_t reg)
{
    dut->rd_sel[port] = reg;
    dut->is_8_bit = 1;
    cycle();
}

void RegisterFileTestbench::trigger_read16(int port, uint8_t reg)
{
    dut->rd_sel[port] = reg;
    dut->is_8_bit = 0;
    cycle();
}

uint16_t RegisterFileTestbench::get_value(int port) const
{
    return dut->rd_val[port];
}

class RegisterFileTestFixture : public ::testing::Test {
public:
    RegisterFileTestFixture()
        : tb(&register_file) {}
protected:
    VRegisterFile register_file;
    RegisterFileTestbench tb;
};

TEST_F(RegisterFileTestFixture, read_write_8_bit)
{
    for (auto r = 0; r < 8; ++r)
        tb.write8(r, r);

    for (auto r = 0; r < 8; ++r) {
        tb.trigger_read8(0, r);
        EXPECT_EQ(r, tb.get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, read_write_16_bit)
{
    for (auto r = 0; r < 8; ++r)
        tb.write16(r, r | ((r << 8) + 1));

    for (auto r = 0; r < 8; ++r) {
        tb.trigger_read16(0, r);
        EXPECT_EQ(r | ((r << 8) + 1), tb.get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, reset)
{
    for (auto r = 0; r < 8; ++r)
        tb.write16(r, ~r);

    tb.reset();

    for (auto r = 0; r < 8; ++r) {
        tb.trigger_read16(0, r);
        EXPECT_EQ(0, tb.get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, read_during_write)
{
    tb.write16(0, 0x1234);
    tb.trigger_read16(0, 0);
    ASSERT_EQ(0x1234, tb.get_value(0));
}

TEST_F(RegisterFileTestFixture, high_low_16)
{
    for (auto r = 0; r < 8; ++r)
        tb.write16(r, r | (~r << 8));

    for (auto r = 0; r < 4; ++r) {
        tb.trigger_read8(0, r);
        EXPECT_EQ(r, tb.get_value(0));
        tb.trigger_read8(0, r + 4);
        EXPECT_EQ(~r & 0xff, tb.get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, multiple_reads)
{
    tb.write16(0, 0xdead);
    tb.write16(1, 0xbeef);

    tb.dut->rd_sel[0] = 1;
    tb.dut->rd_sel[1] = 0;
    tb.cycle();

    EXPECT_EQ(0xbeef, tb.get_value(0));
    EXPECT_EQ(0xdead, tb.get_value(1));
}
