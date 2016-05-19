#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, PushRegFF)
{
    // push ax
    set_instruction({ 0xff, 0xf0 });

    write_reg(AX, 0xaa55);
    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem<uint16_t>(0x0fe));
}

TEST_F(EmulateFixture, PushRegFFInvalidReg)
{
    // push ax
    set_instruction({ 0xff, 0xf8 });

    write_reg(AX, 0xaa55);
    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x100, read_reg(SP));
    ASSERT_EQ(0x0, read_mem<uint16_t>(0x0fe));
}

TEST_F(EmulateFixture, PushMemFF)
{
    // push [1234]
    set_instruction({ 0xff, 0x36, 0x34, 0x12 });

    write_mem<uint16_t>(0x1234, 0xaa55);
    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem<uint16_t>(0x0fe));
}

TEST_F(EmulateFixture, PushReg5X)
{
    // push r
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_reg(reg, 0x0100 + i);
        write_reg(SP, 0x0100);

        set_instruction({ static_cast<uint8_t>(0x50 + i) });
        emulate();

        ASSERT_EQ(0x0fe, read_reg(SP));
        if (reg != SP)
            ASSERT_EQ(0x0100 + i, read_mem<uint16_t>(0x0fe));
        else
            ASSERT_EQ(0x0100, read_mem<uint16_t>(0x0fe));
    }
}

TEST_F(EmulateFixture, PushSR)
{
    // push sr
    for (uint8_t i = 0; i < 4; ++i) {
        write_reg(SS, 0);
        auto reg = static_cast<GPR>(static_cast<int>(ES) + i);
        write_reg(reg, 0x0100 + i);
        write_reg(SP, 0x0100);

        set_instruction({ static_cast<uint8_t>((i << 3) | 0x6) });
        emulate();

        ASSERT_EQ(0x0fe, read_reg(SP));
        if (reg == SS)
            ASSERT_EQ(0x0100 + i, read_mem<uint16_t>(((0x0100 + i) << 4) + 0x0fe));
        else
            ASSERT_EQ(0x0100 + i, read_mem<uint16_t>(0x0fe));
    }
}

TEST_F(EmulateFixture, PopRegFF)
{
    // pop ax
    set_instruction({ 0x8f, 0xc0 });

    write_mem<uint16_t>(0x0fe, 0xaa55);
    write_reg(SP, 0x0fe);

    emulate();

    ASSERT_EQ(0x100, read_reg(SP));
    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, PopMemFF)
{
    // pop [1234]
    set_instruction({ 0x8f, 0x06, 0x34, 0x12 });

    write_mem<uint16_t>(0x0fe, 0xaa55);
    write_reg(SP, 0x0fe);

    emulate();

    ASSERT_EQ(0x100, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem<uint16_t>(0x1234));
}

TEST_F(EmulateFixture, PopRegFFInvalidReg)
{
    // pop ax
    set_instruction({ 0x8f, 0xc8 });

    write_mem<uint16_t>(0x0fe, 0xaa55);
    write_reg(SP, 0x0fe);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0x0, read_reg(AX));
}

TEST_F(EmulateFixture, PopReg5X)
{
    // pop r
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_mem<uint16_t>(0x0fe, 0x0100 + i);
        write_reg(SP, 0x0fe);

        set_instruction({ static_cast<uint8_t>(0x58 + i) });
        emulate();

        if (reg != SP)
            ASSERT_EQ(0x100, read_reg(SP));
        ASSERT_EQ(0x0100 + i, read_reg(reg));
    }
}

TEST_F(EmulateFixture, PopSR)
{
    // pop sr
    for (uint8_t i = 0; i < 4; ++i) {
        write_reg(SS, 0);

        auto reg = static_cast<GPR>(static_cast<int>(ES) + i);
        write_mem<uint16_t>(0x0fe, 0x0100 + i);
        write_reg(SP, 0x0fe);

        set_instruction({ static_cast<uint8_t>((i << 3) | 0x7) });
        emulate();

        ASSERT_EQ(0x100, read_reg(SP));
        ASSERT_EQ(0x0100 + i, read_reg(reg));
    }
}
