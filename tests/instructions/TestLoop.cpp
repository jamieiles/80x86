#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, LoopNotTaken)
{
    write_reg(CX, 1);
    write_reg(IP, 0x100);

    // loop ip-3
    set_instruction({0xe2, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);

    // loop ip-3
    set_instruction({0xe2, 0xfd});

    emulate();

    ASSERT_EQ(0x0ff, read_reg(IP));
}

TEST_F(EmulateFixture, LoopeNotTakenNonZero)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(0);

    // loope ip-3
    set_instruction({0xe1, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopeNotTaken)
{
    write_reg(CX, 1);
    write_reg(IP, 0x100);
    write_flags(ZF);

    // loope ip-3
    set_instruction({0xe1, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopeTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(ZF);

    // loope ip-3
    set_instruction({0xe1, 0xfd});

    emulate();

    ASSERT_EQ(0x0ff, read_reg(IP));
}

TEST_F(EmulateFixture, LoopnzNotTakenNonZero)
{
    write_reg(CX, 1);
    write_reg(IP, 0x100);
    write_flags(0);

    // loopnz ip-3
    set_instruction({0xe0, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopnzNotTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(ZF);

    // loopnz ip-3
    set_instruction({0xe0, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopnzTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(0);

    // loopnz ip-3
    set_instruction({0xe0, 0xfd});

    emulate();

    ASSERT_EQ(0x0ff, read_reg(IP));
}
