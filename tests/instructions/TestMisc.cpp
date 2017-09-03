#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "config.h"

TEST_F(EmulateFixture, Wait)
{
    // wait
    set_instruction({ 0x9b });

    emulate();
}

TEST_F(EmulateFixture, EscReg)
{
    // esc
    for (uint8_t v = 0xd8; v <= 0xdf; ++v) {
        reset();

        write_mem16(VEC_ESCAPE + 2, 0x8000, CS); // CS
        write_mem16(VEC_ESCAPE + 0, 0x0100, CS); // IP

        write_reg(SP, 0x100);
        write_reg(CS, 0x7c00);
        write_reg(IP, 0x0001);

        set_instruction({ v, 0xc0 });
        emulate();

        if (S80X86_TRAP_ESCAPE) {
            EXPECT_EQ(read_reg(CS), 0x8000);
            EXPECT_EQ(read_reg(IP), 0x0100);
            EXPECT_EQ(read_reg(SP), 0x0100 - 6);

            EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS);
            EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
            // Return to the same instruction
            EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0001);
        } else {
            EXPECT_EQ(read_reg(CS), 0x7c00);
            EXPECT_EQ(read_reg(IP), 0x0001 +  2);
        }
    }
}

TEST_F(EmulateFixture, EscMem)
{
    // esc
    for (uint8_t v = 0xd8; v <= 0xdf; ++v) {
        reset();

        write_mem16(VEC_ESCAPE + 2, 0x8000, CS); // CS
        write_mem16(VEC_ESCAPE + 0, 0x0100, CS); // IP

        write_reg(SP, 0x100);
        write_reg(CS, 0x7c00);
        write_reg(IP, 0x0001);

        set_instruction({ v, 0x87, 0x34, 0x12 });
        emulate();

        if (S80X86_TRAP_ESCAPE) {
            EXPECT_EQ(read_reg(CS), 0x8000);
            EXPECT_EQ(read_reg(IP), 0x0100);
            EXPECT_EQ(read_reg(SP), 0x0100 - 6);

            EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS);
            EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
            // Return to the same instruction
            EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0001);
        } else {
            EXPECT_EQ(read_reg(CS), 0x7c00);
            EXPECT_EQ(read_reg(IP), 0x0001 +  4);
        }
    }
}

TEST_F(EmulateFixture, EscMemSegmentOverride)
{
    write_mem16(VEC_ESCAPE + 2, 0x8000, CS); // CS
    write_mem16(VEC_ESCAPE + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);

    // es: esc
    set_instruction({ 0x26, 0xd8, 0x87, 0x34, 0x12 });
    emulate();

    if (S80X86_TRAP_ESCAPE) {
        EXPECT_EQ(read_reg(CS), 0x8000);
        EXPECT_EQ(read_reg(IP), 0x0100);
        EXPECT_EQ(read_reg(SP), 0x0100 - 6);

        EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS);
        EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
        // Return to the same instruction
        EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0001);
    } else {
        EXPECT_EQ(read_reg(CS), 0x7c00);
        EXPECT_EQ(read_reg(IP), 0x0001 +  5);
    }
}
