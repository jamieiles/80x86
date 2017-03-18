#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, Hlt)
{
    // hlt
    set_instruction({ 0xf4 });

    emulate();
}

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
        set_instruction({ v, 0xc0 });
        emulate();
    }
}

TEST_F(EmulateFixture, EscMem)
{
    // esc
    for (uint8_t v = 0xd8; v <= 0xdf; ++v) {
        reset();
        set_instruction({ v, 0x87, 0x34, 0x12 });
        emulate();
    }
}
