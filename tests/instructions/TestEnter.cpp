#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, EnterNoNestNoAlloc)
{
    write_reg(SP, 0x0100);
    write_reg(BP, 0x0200);

    set_instruction({ 0xc8, 0x00, 0x00, 0x00 });

    emulate();

    EXPECT_EQ(0x00fe, read_reg(BP));
    EXPECT_EQ(0x00fe, read_reg(SP));
}

TEST_F(EmulateFixture, EnterNoNestAlloc10)
{
    write_reg(SP, 0x0100);
    write_reg(BP, 0x0200);

    set_instruction({ 0xc8, 0x10, 0x00, 0x00 });

    emulate();

    EXPECT_EQ(0x00fe, read_reg(BP));
    EXPECT_EQ(0x00fe - 0x10, read_reg(SP));
}

TEST_F(EmulateFixture, EnterNest4)
{
    write_reg(SP, 0x0100);
    write_reg(BP, 0x0200);

    for (int i = 0; i < 3; ++i)
        write_mem16(0x200 - (i + 1) * sizeof(uint16_t),
                    i + 1, SS);

    set_instruction({ 0xc8, 0x00, 0x00, 0x04 });

    emulate();

    auto addr = 0x0100 - 2;
    EXPECT_EQ(0x0200, read_mem16(addr, SS));
    for (int i = 0; i < 3; ++i) {
        addr -= 2;
        EXPECT_EQ(i + 1, read_mem16(addr, SS));
    }

    addr -= 2;
    EXPECT_EQ(0x00fe, read_mem16(addr, SS));

    EXPECT_EQ(read_reg(BP), 0x0fe);
    EXPECT_EQ(read_reg(SP), addr);
}

TEST_F(EmulateFixture, EnterNestMask32)
{
    write_reg(SP, 0x0100);
    write_reg(BP, 0x0200);

    for (int i = 0; i < 128; ++i)
        write_mem16(0x200 - (i + 1) * sizeof(uint16_t),
                    0xa5a5, SS);

    set_instruction({ 0xc8, 0x00, 0x00, 0xff });

    emulate();

    auto addr = 0x0100 - 2;
    int count = 0;
    for (int i = 0; i < 128; ++i) {
        addr -= 2;

        if (read_mem16(addr, SS) != 0xa5a5)
            break;

        ++count;
        EXPECT_LT(i, 32);
    }

    EXPECT_EQ(count, 30);
}
