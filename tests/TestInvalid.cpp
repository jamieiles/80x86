#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, InvalidRegF6)
{
    set_instruction({ 0xf6, 1 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegF7)
{
    set_instruction({ 0xf7, 1 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegD0)
{
    set_instruction({ 0xd0, 6 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegD1)
{
    set_instruction({ 0xd1, 6 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegD2)
{
    set_instruction({ 0xd2, 6 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegD3)
{
    set_instruction({ 0xd3, 6 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegFE)
{
    set_instruction({ 0xfe, 2 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}
