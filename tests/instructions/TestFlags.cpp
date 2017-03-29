#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, Lahf)
{
    set_instruction({ 0x9f });
    write_flags(0xd7);

    emulate();

    ASSERT_EQ(0xd7, read_reg(AH));
}

TEST_F(EmulateFixture, Sahf)
{
    set_instruction({ 0x9e });
    uint16_t old_flags = CF;
    write_flags(old_flags);
    write_reg(AH, CF | PF | AF);

    emulate();

    ASSERT_EQ(CF | PF | AF | FLAGS_STUCK_BITS, read_flags());
}

TEST_F(EmulateFixture, Pushf)
{
    write_flags(0x00d7);
    write_reg(SP, 0x0100);
    set_instruction({ 0x9c });

    emulate();

    ASSERT_EQ(read_reg(SP), 0x00fe);
    ASSERT_EQ(0x00d7, read_mem<uint16_t>(0x00fe, SS));
}

TEST_F(EmulateFixture, Popf)
{
    write_reg(SP, 0x00fe);
    write_mem<uint16_t>(0x00fe, 0x00d7, SS);
    set_instruction({ 0x9d });

    emulate();

    ASSERT_EQ(read_reg(SP), 0x0100);
    ASSERT_EQ(0x00d7, read_flags());
}
