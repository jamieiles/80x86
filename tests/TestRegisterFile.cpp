#include <gtest/gtest.h>
#include "RegisterFile.h"

TEST(RegisterFile, hlx_update_propogated)
{
    RegisterFile rf;

    rf.set(AX, 0x55aa);
    ASSERT_EQ(0x55, rf.get(AH));
    ASSERT_EQ(0xaa, rf.get(AL));
    ASSERT_EQ(0x55aa, rf.get(AX));

    rf.set(AH, 0xde);
    rf.set(AL, 0xad);
    ASSERT_EQ(0xde, rf.get(AH));
    ASSERT_EQ(0xad, rf.get(AL));
    ASSERT_EQ(0xdead, rf.get(AX));
}

TEST(RegisterFile, reset_clears)
{
    RegisterFile rf;

    rf.set(AX, 0x1234);
    ASSERT_EQ(0x1234, rf.get(AX));
    rf.set_flags(0x00ff);
    // Bit 15 is always 1 on 8086
    ASSERT_EQ(0x80ff, rf.get_flags());

    rf.reset();
    ASSERT_EQ(0x0000, rf.get(AX));
    // Bit 15 is always 1 on 8086
    ASSERT_EQ(FLAGS_STUCK_BITS, rf.get_flags());
}
