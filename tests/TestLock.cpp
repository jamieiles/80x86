#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, Lock)
{
    // lock nop
    // an invalid encoding but fine on 8086
    set_instruction({ 0xf0, 0x90 });

    emulate();
}
