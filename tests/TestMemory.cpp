#include <gtest/gtest.h>
#include "Memory.h"

TEST(Memory, read_write_16)
{
    Memory m;

    m.write<uint16_t>(0, 0xface);
    ASSERT_EQ(0xface, m.read<uint16_t>(0));

    // Unaligned
    m.write<uint16_t>(1, 0xface);
    ASSERT_EQ(0xface, m.read<uint16_t>(1));
}
