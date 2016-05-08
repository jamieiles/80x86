#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, Lahf)
{
    set_instruction({ 0x9f });
    write_flags(0xd5);

    emulate();

    ASSERT_EQ(0xd5, read_reg(AH));
}

TEST_F(EmulateFixture, Sahf)
{
    set_instruction({ 0x9e });
    uint16_t old_flags = (1 << 15) | (1 << 11);
    write_flags(old_flags);
    write_reg(AH, 0xd5);

    emulate();

    ASSERT_EQ(old_flags | 0xd5, read_flags());
}

TEST_F(EmulateFixture, Pushf)
{
    write_flags(0x80d5);
    write_reg(SP, 0x0100);
    set_instruction({ 0x9c });

    emulate();

    ASSERT_EQ(read_reg(SP), 0x00fe);
    ASSERT_EQ(0x80d5, read_mem<uint16_t>(0x00fe));
}

TEST_F(EmulateFixture, Popf)
{
    write_reg(SP, 0x00fe);
    write_mem<uint16_t>(0x00fe, 0x80d5);
    set_instruction({ 0x9d });

    emulate();

    ASSERT_EQ(read_reg(SP), 0x0100);
    ASSERT_EQ(0x80d5, read_flags());
}
