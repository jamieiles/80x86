#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, Leave)
{
    write_reg(SP, 0x0000);
    write_reg(BP, 0x00fe);
    write_mem16(0x00fe, 0x01fe, SS);

    set_instruction({0xc9});

    emulate();

    EXPECT_EQ(0x01fe, read_reg(BP));
    EXPECT_EQ(0x0100, read_reg(SP));
}
