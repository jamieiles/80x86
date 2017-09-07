#include <gtest/gtest.h>
#include <VRegisterFile.h>

#include "VerilogTestbench.h"
#include "RTLCPU.h"

class RegisterFileTestFixture : public VerilogTestbench<VRegisterFile>,
                                public ::testing::Test
{
public:
    void write8(uint8_t reg, uint8_t v);
    void write16(uint8_t reg, uint16_t v);
    void trigger_read8(int port, uint8_t reg);
    void trigger_read16(int port, uint8_t reg);
    uint16_t get_value(int port);
};

void RegisterFileTestFixture::write8(uint8_t reg, uint8_t v)
{
    after_n_cycles(0, [&] {
        this->dut.wr_sel = reg;
        this->dut.wr_val = v;
        this->dut.wr_en = 1;
        this->dut.is_8_bit = 1;
        after_n_cycles(1, [&] { dut.wr_en = 0; });
    });
    cycle();
}

void RegisterFileTestFixture::write16(uint8_t reg, uint16_t v)
{
    after_n_cycles(0, [&] {
        this->dut.wr_sel = reg;
        this->dut.wr_val = v;
        this->dut.wr_en = 1;
        this->dut.is_8_bit = 0;
        after_n_cycles(1, [&] { dut.wr_en = 0; });
    });
    cycle();
}

void RegisterFileTestFixture::trigger_read8(int port, uint8_t reg)
{
    after_n_cycles(0, [&] {
        dut.rd_sel[port] = reg;
        dut.is_8_bit = 1;
    });
    cycle();
}

void RegisterFileTestFixture::trigger_read16(int port, uint8_t reg)
{
    after_n_cycles(0, [&] {
        dut.rd_sel[port] = reg;
        dut.is_8_bit = 0;
    });
    cycle();
}

uint16_t RegisterFileTestFixture::get_value(int port)
{
    cycle();
    return dut.rd_val[port];
}

TEST_F(RegisterFileTestFixture, read_write_8_bit)
{
    for (auto r = 0; r < 8; ++r)
        write8(r, r);

    for (auto r = 0; r < 8; ++r) {
        trigger_read8(0, r);
        EXPECT_EQ(r, get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, read_write_16_bit)
{
    for (auto r = 0; r < 8; ++r)
        write16(r, r | ((r << 8) + 1));

    for (auto r = 0; r < 8; ++r) {
        trigger_read16(0, r);
        EXPECT_EQ(r | ((r << 8) + 1), get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, read_during_write)
{
    write16(0, 0x1234);
    trigger_read16(0, 0);
    ASSERT_EQ(0x1234, get_value(0));
}

TEST_F(RegisterFileTestFixture, high_low_16)
{
    for (auto r = 0; r < 8; ++r)
        write16(r, r | (~r << 8));

    for (auto r = 0; r < 4; ++r) {
        trigger_read8(0, r);
        EXPECT_EQ(r, get_value(0));
        trigger_read8(0, r + 4);
        EXPECT_EQ(~r & 0xff, get_value(0));
    }
}

TEST_F(RegisterFileTestFixture, multiple_reads)
{
    write16(0, 0xdead);
    write16(1, 0xbeef);

    dut.rd_sel[0] = 1;
    dut.rd_sel[1] = 0;
    cycle();

    EXPECT_EQ(0xbeef, get_value(0));
    EXPECT_EQ(0xdead, get_value(1));
}

class CoreFixture : public RTLCPU<verilator_debug_enabled>,
                    public ::testing::Test
{
public:
    CoreFixture() : RTLCPU(current_test_name()) {}
};
TEST_F(CoreFixture, RegisterFileReset)
{
    for (auto r = static_cast<int>(AX); r <= static_cast<int>(DI); ++r) {
        write_reg(static_cast<GPR>(r), ~r);
        EXPECT_EQ(read_reg(static_cast<GPR>(r)), static_cast<uint16_t>(~r));
    }

    reset();

    for (auto r = static_cast<int>(AX); r <= static_cast<int>(DI); ++r)
        EXPECT_EQ(read_reg(static_cast<GPR>(r)), 0);
}
