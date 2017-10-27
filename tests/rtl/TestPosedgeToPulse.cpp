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
#include <VPosedgeToPulse.h>

#include "VerilogTestbench.h"

class PosedgeToPulseTestFixture : public VerilogTestbench<VPosedgeToPulse>,
                                  public ::testing::Test
{
public:
    PosedgeToPulseTestFixture();
};

PosedgeToPulseTestFixture::PosedgeToPulseTestFixture()
{
    reset();
}

TEST_F(PosedgeToPulseTestFixture, IdleNoPulse)
{
    for (auto i = 0; i < 32; ++i) {
        cycle();
        EXPECT_EQ(dut.q, 0);
    }
}

TEST_F(PosedgeToPulseTestFixture, PosedgeTriggersOnce)
{
    EXPECT_EQ(dut.q, 0);

    after_n_cycles(0, [&] { this->dut.d = 1; });
    cycle(2);
    EXPECT_EQ(this->dut.q, 1);

    for (auto i = 0; i < 32; ++i) {
        cycle();
        EXPECT_EQ(this->dut.q, 0);
    }
}

TEST_F(PosedgeToPulseTestFixture, PosedgePulseTriggersN)
{
    EXPECT_EQ(dut.q, 0);

    for (auto i = 0; i < 8; ++i) {
        after_n_cycles(0, [&] {
            this->dut.d = 1;
            after_n_cycles(1, [&] { this->dut.d = 0; });
        });
        cycle(2);
        EXPECT_EQ(this->dut.q, 1);

        for (auto j = 0; j < 32; ++j) {
            cycle();
            EXPECT_EQ(this->dut.q, 0);
        }
    }
}
