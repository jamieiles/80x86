#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, InFixed8)
{
    // in al, 0x80
    set_instruction({ 0xe4, 0x80 });
    write_io8(0x80, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_reg(AL));
}

TEST_F(EmulateFixture, InFixed16)
{
    // in ax, 0x10
    set_instruction({ 0xe5, 0x10 });
    write_io16(0x10, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, InVariable8)
{
    // in al, dx
    set_instruction({ 0xec });
    write_reg(DX, 0x0100);
    write_io8(0x0100, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_reg(AL));
}

TEST_F(EmulateFixture, InVariable16)
{
    // in ax, dx
    set_instruction({ 0xed });
    write_reg(DX, 0x0100);
    write_io16(0x0100, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, OutFixed8)
{
    // out 0x10, al
    set_instruction({ 0xe6, 0x10 });
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_io8(0x10));
}

TEST_F(EmulateFixture, OutFixed16)
{
    // out 0x10, ax
    set_instruction({ 0xe7, 0x10 });
    write_reg(AX, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_io16(0x10));
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

    ASSERT_EQ(0xaa55, read_io16(0x10));
}

TEST_F(EmulateFixture, OutVariable8)
{
    // out dx, al
    set_instruction({ 0xee });
    write_reg(DX, 0x0100);
    write_reg(AX, 0xaa);

    emulate();

    ASSERT_EQ(0xaa, read_io8(0x0100));
}

TEST_F(EmulateFixture, OutVariable16)
{
    // out dx, ax
    set_instruction({ 0xef });
    write_reg(DX, 0x0100);
    write_reg(AX, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_io16(0x0100));
}

TEST_F(EmulateFixture, Outsb)
{
    write_flags(0);
    write_reg(SI, 0x800);
    write_vector8(0x800, {0, 1, 2, 3}, DS);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep outsb
    set_instruction({ 0xf2, 0x6e });

    emulate();

    auto expected = std::vector<uint16_t>{ 0, 1, 2 };
    EXPECT_EQ(io100.get_write_vals(), expected);
}

TEST_F(EmulateFixture, OutsbDec)
{
    write_flags(DF);
    write_reg(SI, 0x802);
    write_vector8(0x800, {0, 1, 2, 3}, DS);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep outsb
    set_instruction({ 0xf2, 0x6e });

    emulate();

    auto expected = std::vector<uint16_t>{ 2, 1, 0 };
    EXPECT_EQ(io100.get_write_vals(), expected);
}

TEST_F(EmulateFixture, Outsw)
{
    write_flags(0);
    write_reg(SI, 0x800);
    write_vector16(0x800, {0x0101, 0x0202, 0x0303, 0x0404}, DS);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep outsw
    set_instruction({ 0xf2, 0x6f });

    emulate();

    auto expected = std::vector<uint16_t>{ 0x0101, 0x0202, 0x0303 };
    EXPECT_EQ(io100.get_write_vals(), expected);
}

TEST_F(EmulateFixture, OutswDec)
{
    write_flags(DF);
    write_reg(SI, 0x804);
    write_vector16(0x800, {0x0101, 0x0202, 0x0303, 0x0404}, DS);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep outsw
    set_instruction({ 0xf2, 0x6f });

    emulate();

    auto expected = std::vector<uint16_t>{ 0x0303, 0x0202, 0x0101 };
    EXPECT_EQ(io100.get_write_vals(), expected);
}

TEST_F(EmulateFixture, Insb)
{
    write_flags(0);
    write_reg(DI, 0x800);
    write_io8(0x100, 0xaa);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep insb
    set_instruction({ 0xf2, 0x6c });

    emulate();

    for (auto i = 0; i < 3; ++i)
        EXPECT_EQ(read_mem8(0x800 + i, ES), 0xaa);
    EXPECT_EQ(read_mem8(0x803, ES),  0x00);
}

TEST_F(EmulateFixture, InsbDec)
{
    write_flags(DF);
    write_reg(DI, 0x802);
    write_io8(0x100, 0xaa);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep insb
    set_instruction({ 0xf2, 0x6c });

    emulate();

    for (auto i = 0; i < 3; ++i)
        EXPECT_EQ(read_mem8(0x800 + i, ES), 0xaa);
    EXPECT_EQ(read_mem8(0x7ff, ES),  0x00);
}

TEST_F(EmulateFixture, Insw)
{
    write_flags(0);
    write_reg(DI, 0x800);
    write_io16(0x100, 0xaa55);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep insw
    set_instruction({ 0xf2, 0x6d });

    emulate();

    for (auto i = 0; i < 3; ++i)
        EXPECT_EQ(read_mem16(0x800 + i * 2, ES), 0xaa55);
    EXPECT_EQ(read_mem8(0x806, ES),  0x00);
}

TEST_F(EmulateFixture, InswDec)
{
    write_flags(DF);
    write_reg(DI, 0x804);
    write_io16(0x100, 0xaa55);

    write_reg(DX, 0x100);
    write_reg(CX, 3);

    // rep insw
    set_instruction({ 0xf2, 0x6d });

    emulate();

    for (auto i = 0; i < 3; ++i)
        EXPECT_EQ(read_mem16(0x800 + i * 2, ES), 0xaa55);
    EXPECT_EQ(read_mem8(0x7fe, ES),  0x00);
}
