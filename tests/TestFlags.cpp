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
