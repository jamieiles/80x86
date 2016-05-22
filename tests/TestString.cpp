#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, ScasbInc)
{
    write_reg(AL, 0);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);
    write_cstring(0x800, "8086");

    // repne scasb
    set_instruction({ 0xf2, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x805);
}

TEST_F(EmulateFixture, ScasbDec)
{
    write_flags(DF);
    write_reg(AL, '1');
    write_reg(DI, 0x803);
    write_reg(CX, 0xff);
    write_cstring(0x800, "1234");

    // repne scasb
    set_instruction({ 0xf2, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7ff);
}

TEST_F(EmulateFixture, ScasbIncRepe)
{
    write_reg(AL, 'a');
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);
    write_cstring(0x800, "aaab");

    // repe scasb
    set_instruction({ 0xf3, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x804);
}

TEST_F(EmulateFixture, ScasbDecRepe)
{
    write_flags(DF);
    write_reg(AL, 'b');
    write_reg(DI, 0x803);
    write_reg(CX, 0xff);
    write_cstring(0x800, "abbb");

    // repe scasb
    set_instruction({ 0xf3, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7ff);
}

TEST_F(EmulateFixture, ScaswInc)
{
    write_reg(AX, 0);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);

    for (int i = 0; i < 2; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repne scasw
    set_instruction({ 0xf2, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x806);
}

TEST_F(EmulateFixture, ScaswDec)
{
    write_flags(DF);
    write_reg(AX, 0);
    write_reg(DI, 0x806);
    write_reg(CX, 0xff);

    for (int i = 0; i < 4; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repne scasw
    set_instruction({ 0xf2, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7fc);
}

TEST_F(EmulateFixture, ScaswIncRepe)
{
    write_reg(AX, 0xaa55);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);

    for (int i = 0; i < 4; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repe scasw
    set_instruction({ 0xf3, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x80a);
}

TEST_F(EmulateFixture, ScaswDecRepe)
{
    write_flags(DF);
    write_reg(AX, 0xaa55);
    write_reg(DI, 0x806);
    write_reg(CX, 0xff);

    for (int i = 0; i < 4; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repe scasw
    set_instruction({ 0xf3, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7fc);
}

TEST_F(EmulateFixture, MovsbInc)
{
    write_reg(AL, 0);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 0x4);
    write_cstring(0x800, "8086");

    // repne movsb
    set_instruction({ 0xf3, 0xa4 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x404);
    ASSERT_EQ(read_reg(SI), 0x804);
    ASSERT_EQ(read_cstring(0x400), "8086");
}

TEST_F(EmulateFixture, MovsbDec)
{
    write_flags(DF);
    write_reg(AL, 0);
    write_reg(SI, 0x803);
    write_reg(DI, 0x403);
    write_reg(CX, 0x4);
    write_cstring(0x800, "8086");

    // repne movsb
    set_instruction({ 0xf3, 0xa4 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x3ff);
    ASSERT_EQ(read_reg(SI), 0x7ff);
    ASSERT_EQ(read_cstring(0x400), "8086");
}

TEST_F(EmulateFixture, MovswInc)
{
    write_reg(AL, 0);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 0x2);
    write_mem<uint16_t>(0x800, 0xaa55);
    write_mem<uint16_t>(0x802, 0x55aa);

    // repne movsw
    set_instruction({ 0xf3, 0xa5 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x404);
    ASSERT_EQ(read_reg(SI), 0x804);
    ASSERT_EQ(read_mem<uint16_t>(0x400), 0xaa55);
    ASSERT_EQ(read_mem<uint16_t>(0x402), 0x55aa);
}

TEST_F(EmulateFixture, MovswDec)
{
    write_flags(DF);
    write_reg(AL, 0);
    write_reg(SI, 0x802);
    write_reg(DI, 0x402);
    write_reg(CX, 0x2);
    write_mem<uint16_t>(0x800, 0xaa55);
    write_mem<uint16_t>(0x802, 0x55aa);

    // repne movsw
    set_instruction({ 0xf3, 0xa5 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x3fe);
    ASSERT_EQ(read_reg(SI), 0x7fe);
    ASSERT_EQ(read_mem<uint16_t>(0x400), 0xaa55);
    ASSERT_EQ(read_mem<uint16_t>(0x402), 0x55aa);
}
