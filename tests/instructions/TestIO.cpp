#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, InFixed8)
{
    // in al, 0x80
    set_instruction({ 0xe4, 0x80 });
    write_io<uint8_t>(0x80, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_reg(AL));
}

TEST_F(EmulateFixture, InFixed16)
{
    // in ax, 0x10
    set_instruction({ 0xe5, 0x10 });
    write_io<uint16_t>(0x10, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, InVariable8)
{
    // in al, dx
    set_instruction({ 0xec });
    write_reg(DX, 0x0100);
    write_io<uint8_t>(0x0100, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_reg(AL));
}

TEST_F(EmulateFixture, InVariable16)
{
    // in ax, dx
    set_instruction({ 0xed });
    write_reg(DX, 0x0100);
    write_io<uint16_t>(0x0100, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, OutFixed8)
{
    // out 0x10, al
    set_instruction({ 0xe6, 0x10 });
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_io<uint8_t>(0x10));
}

TEST_F(EmulateFixture, OutFixed16)
{
    // out 0x10, ax
    set_instruction({ 0xe7, 0x10 });
    write_reg(AX, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_io<uint16_t>(0x10));
}

TEST_F(EmulateFixture, IOIgnoresSegments)
{
    // out 0x10, ax
    set_instruction({ 0xe7, 0x10 });
    write_reg(AX, 0xaa55);
    write_reg(DS, 0x1234);
    write_reg(ES, 0x1234);
    write_reg(SS, 0x1234);

    emulate();

    ASSERT_EQ(0xaa55, read_io<uint16_t>(0x10));
}

TEST_F(EmulateFixture, OutVariable8)
{
    // out dx, al
    set_instruction({ 0xee });
    write_reg(DX, 0x0100);
    write_reg(AX, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_io<uint8_t>(0x0100));
}

TEST_F(EmulateFixture, OutVariable16)
{
    // out dx, ax
    set_instruction({ 0xef });
    write_reg(DX, 0x0100);
    write_reg(AX, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_io<uint16_t>(0x0100));
}
