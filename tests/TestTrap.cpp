#include <gtest/gtest.h>
#include "CPU.h"
#include "EmulateFixture.h"

TEST_F(EmulateFixture, TrapOnInt3)
{
    set_instruction({ 0x90 });
    emulate();

    ASSERT_FALSE(cpu.has_trapped());

    set_instruction({ 0xcc });
    emulate();

    ASSERT_TRUE(cpu.has_trapped());
}
