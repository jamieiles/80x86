#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, Xlat)
{
    write_mem8(0x1000 + 0xaa, 0x55);
    write_reg(BX, 0x1000);
    write_reg(AL, 0xaa);
    set_instruction({ 0xd7 });

    emulate();

    ASSERT_EQ(0x55, read_reg(AL));
}
