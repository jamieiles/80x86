// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

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
    rf.set_flags(0x0000);
    // Bit 1 is always 1 on 8086
    ASSERT_EQ(0xf002, rf.get_flags());

    rf.reset();
    ASSERT_EQ(0x0000, rf.get(AX));
    // Bit 1 is always 1 on 8086
    ASSERT_EQ(FLAGS_STUCK_BITS, rf.get_flags());
}

TEST(RegisterFile, flags_mask)
{
    RegisterFile rf;

    rf.set_flags(CF | AF, CF);

    ASSERT_EQ(CF | FLAGS_STUCK_BITS, rf.get_flags());
}
