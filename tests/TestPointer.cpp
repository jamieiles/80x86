#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, LeaAxBx)
{
    // lea ax, [bx]
    set_instruction({ 0x8d, 0x07 });
    write_reg(BX, 0x1234);

    emulate();

    ASSERT_EQ(0x1234, read_reg(AX));
}

TEST_F(EmulateFixture, LeaAxBxSiDisplacement)
{
    // lea ax, [bx+si+0x1234]
    set_instruction({ 0x8d, 0x80, 0x34, 0x12 });
    write_reg(BX, 0x0400);
    write_reg(SI, 0x0100);

    emulate();

    ASSERT_EQ(0x1234 + 0x0400 + 0x0100, read_reg(AX));
}
