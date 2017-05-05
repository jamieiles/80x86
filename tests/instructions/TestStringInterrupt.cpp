#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

class StringIntTest : public EmulateFixture {
public:
    void setup_nmi_handler();
    void assert_nmi_taken();
    void inject_nmi();
};

void StringIntTest::setup_nmi_handler()
{
    write_mem16(VEC_NMI + 2, 0x8000, CS); // CS
    write_mem16(VEC_NMI + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
}

void StringIntTest::inject_nmi()
{
    cpu->start_instruction();

    // Skip over the opcode_fetch yield point
    wait_for_int_yield();
    cpu->cycle_cpu();

    // yield point inside the string instruction
    wait_for_int_yield();
    cpu->raise_nmi();

    cpu->complete_instruction();
}

void StringIntTest::assert_nmi_taken()
{
    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags() & (TF | IF), 0);
    EXPECT_EQ(read_reg(CS), 0x8000);
    EXPECT_EQ(read_reg(IP), 0x0100);
    EXPECT_EQ(read_reg(SP), 0x0100 - 6);

    EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x0000);
    // The instruction will be retried
    EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x1000);
}

// These tests only run on the RTL CPU where we know that we can start single
// stepping and then go through cycle by cycle and then inject an NMI.

TEST_F(StringIntTest, ScasbInt)
{
    setup_nmi_handler();

    write_reg(AL, 0);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);
    write_cstring(0x800, "8086", ES);

    // repne scasb
    set_instruction({ 0xf2, 0xae });

    inject_nmi();

    ASSERT_EQ(read_reg(DI), 0x801);

    assert_nmi_taken();
}

TEST_F(StringIntTest, ScaswInt)
{
    setup_nmi_handler();

    write_reg(AX, 0);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);

    for (int i = 0; i < 2; ++i)
        write_mem16(0x800 + i * 2, 0xaa55, ES);
    write_mem16(0x800 + 2 * 2, 0x0000, ES);

    // repne scasw
    set_instruction({ 0xf2, 0xaf });

    inject_nmi();

    ASSERT_EQ(read_reg(DI), 0x802);

    assert_nmi_taken();
}

TEST_F(StringIntTest, MovsbInt)
{
    setup_nmi_handler();

    write_reg(AL, 0);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 0x4);
    write_cstring(0x800, "8086");
    write_mem8(0x404, 0, ES);

    // repne movsb
    set_instruction({ 0xf3, 0xa4 });

    inject_nmi();

    ASSERT_EQ(read_reg(DI), 0x401);
    ASSERT_EQ(read_reg(SI), 0x801);

    assert_nmi_taken();
}

TEST_F(StringIntTest, MovswInt)
{
    setup_nmi_handler();

    write_reg(AL, 0);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 0x2);
    write_mem16(0x800, 0xaa55);
    write_mem16(0x802, 0x55aa);

    // repne movsw
    set_instruction({ 0xf3, 0xa5 });

    inject_nmi();

    ASSERT_EQ(read_reg(DI), 0x402);
    ASSERT_EQ(read_reg(SI), 0x802);

    assert_nmi_taken();
}

TEST_F(StringIntTest, CmpsbInt)
{
    setup_nmi_handler();

    write_flags(0);

    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 8);

    for (auto i = 0; i < 8; ++i) {
        write_mem8(0x400 + i, 0, ES);
        write_mem8(0x800 + i, 0);
    }
    write_vector8(0x800, { 0, 1, 2, 4, 5, 6, 7 });
    write_vector8(0x400, { 0, 1, 2, 4, 5, 6, 7 }, ES);

    set_instruction({ 0xf3, 0xa6 });

    inject_nmi();

    ASSERT_EQ(1, read_reg(SI) - 0x800);

    assert_nmi_taken();
}

TEST_F(StringIntTest, CmpswInt)
{
    setup_nmi_handler();

    write_flags(0);

    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 4);

    for (auto i = 0; i < 8; ++i) {
        write_mem8(0x400 + i, 0, ES);
        write_mem8(0x800 + i, 0);
    }
    write_vector8(0x800, { 0, 1, 2, 4, 5, 6, 7 });
    write_vector8(0x400, { 0, 1, 2, 4, 5, 6, 7 }, ES);

    set_instruction({ 0xf3, 0xa7 });

    inject_nmi();

    ASSERT_EQ(2, read_reg(SI) - 0x800);

    assert_nmi_taken();
}

TEST_F(StringIntTest, LodsbInt)
{
    setup_nmi_handler();

    write_flags(0);
    write_reg(SI, 0x800);
    write_cstring(0x800, "foo");
    write_reg(CX, 3);

    set_instruction({ 0xf2, 0xac });

    inject_nmi();

    ASSERT_EQ(read_reg(SI), 0x801);

    assert_nmi_taken();
}

TEST_F(StringIntTest, LodswInt)
{
    setup_nmi_handler();

    write_flags(0);
    write_reg(SI, 0x800);
    write_vector16(0x800, { 0x1234, 0x5678 });
    write_reg(CX, 2);

    set_instruction({ 0xf2, 0xad });

    inject_nmi();

    ASSERT_EQ(read_reg(SI), 0x802);

    assert_nmi_taken();
}

TEST_F(StringIntTest, StosbInt)
{
    setup_nmi_handler();

    write_flags(0);
    write_reg(DI, 0x800);
    write_mem8(0x803, 0);
    write_reg(AL, 'a');
    write_reg(CX, 3);
    write_vector8(0x800, {0, 0, 0, 0, 0, 0, 0, 0}, ES);

    set_instruction({ 0xf2, 0xaa });

    inject_nmi();

    ASSERT_EQ(read_reg(DI), 0x801);

    assert_nmi_taken();
}

TEST_F(StringIntTest, StoswInt)
{
    setup_nmi_handler();

    write_flags(0);
    write_reg(DI, 0x800);
    write_reg(AX, 0x6261);
    write_mem16(0x806, 0x0000);
    write_reg(CX, 3);
    write_vector16(0x800, {0, 0, 0, 0, 0, 0, 0, 0}, ES);

    set_instruction({ 0xf2, 0xab });

    inject_nmi();

    ASSERT_EQ(read_reg(DI), 0x802);

    assert_nmi_taken();
}
