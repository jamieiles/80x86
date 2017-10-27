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
#include <VFlags.h>

#include "VerilogTestbench.h"
#include "MicrocodeTypes.h"
#include "RegisterFile.h"

class FlagsTestFixture : public VerilogTestbench<VFlags>, public ::testing::Test
{
public:
    FlagsTestFixture();
};

FlagsTestFixture::FlagsTestFixture()
{
    reset();
}

static int flag_to_update_mask(Flag f)
{
    switch (f) {
    case CF: return 1 << static_cast<int>(UpdateFlags_CF);
    case PF: return 1 << static_cast<int>(UpdateFlags_PF);
    case AF: return 1 << static_cast<int>(UpdateFlags_AF);
    case ZF: return 1 << static_cast<int>(UpdateFlags_ZF);
    case SF: return 1 << static_cast<int>(UpdateFlags_SF);
    case TF: return 1 << static_cast<int>(UpdateFlags_TF);
    case IF: return 1 << static_cast<int>(UpdateFlags_IF);
    case DF: return 1 << static_cast<int>(UpdateFlags_DF);
    case OF: return 1 << static_cast<int>(UpdateFlags_OF);
    default: abort();
    }
}

TEST_F(FlagsTestFixture, update_flags)
{
    ASSERT_EQ(dut.flags_out, FLAGS_STUCK_BITS);

    dut.flags_in = CF | PF | AF | ZF | SF | IF | DF | OF;
    dut.update_flags = 0;
    cycle();

    ASSERT_EQ(dut.flags_out, FLAGS_STUCK_BITS);

    std::vector<Flag> flags = {CF, PF, AF, ZF, SF, TF, IF, DF, OF};
    for (auto f : flags) {
        reset();

        dut.flags_in = f;
        dut.update_flags = flag_to_update_mask(f);
        cycle();

        ASSERT_EQ(dut.flags_out, f | FLAGS_STUCK_BITS);
    }
}
