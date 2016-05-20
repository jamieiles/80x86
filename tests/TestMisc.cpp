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
