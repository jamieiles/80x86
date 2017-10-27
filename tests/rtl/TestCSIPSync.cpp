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
#include <VCSIPSync.h>

#include "VerilogTestbench.h"

class CSIPSyncTestFixture : public VerilogTestbench<VCSIPSync>,
                            public ::testing::Test
{
public:
    CSIPSyncTestFixture();

protected:
    void update_ip(uint16_t newip);
    void update_cs();
    void propagate();
    bool updated;
    uint16_t updated_ip;
};

CSIPSyncTestFixture::CSIPSyncTestFixture() : updated(false), updated_ip(true)
{
    reset();

    periodic(ClockCapture, [&] {
        if (this->dut.update_out) {
            this->updated = true;
            this->updated_ip = this->dut.ip_out;
        }
    });
}

void CSIPSyncTestFixture::update_ip(uint16_t newip)
{
    after_n_cycles(0, [&] {
        this->dut.new_ip = newip;
        this->dut.ip_update = 1;
        after_n_cycles(1, [&] {
            this->dut.ip_update = 0;
            this->dut.new_ip = 0;
        });
    });
    cycle(10);
}

void CSIPSyncTestFixture::update_cs()
{
    after_n_cycles(0, [&] {
        this->dut.cs_update = 1;
        after_n_cycles(1, [&] { this->dut.cs_update = 0; });
    });
    cycle(10);
}

void CSIPSyncTestFixture::propagate()
{
    after_n_cycles(0, [&] {
        this->dut.propagate = 1;
        after_n_cycles(1, [&] { this->dut.propagate = 0; });
    });
    cycle();
}

TEST_F(CSIPSyncTestFixture, NoChangeNoUpdate)
{
    propagate();

    ASSERT_FALSE(updated);
}

TEST_F(CSIPSyncTestFixture, IPPropagateDelayed)
{
    update_ip(0x1234);
    propagate();

    EXPECT_TRUE(updated);
    EXPECT_EQ(updated_ip, 0x1234);

    cycle();
}

TEST_F(CSIPSyncTestFixture, CSPropagateDelayed)
{
    update_cs();
    propagate();

    EXPECT_TRUE(updated);
    EXPECT_EQ(updated_ip, 0);

    cycle();
}

TEST_F(CSIPSyncTestFixture, SimultaneousUpdatePropagateDoesntLatch)
{
    after_n_cycles(0, [&] {
        this->dut.new_ip = 0xf00f;
        this->dut.propagate = 1;
        this->dut.ip_update = 1;
        after_n_cycles(1, [&] {
            this->dut.propagate = 0;
            this->dut.ip_in = 0x1001;
            this->dut.new_ip = 0x1001;
        });
    });
    cycle(1);
    ASSERT_EQ(this->dut.ip_out, 0xf00f);

    propagate();
    ASSERT_EQ(this->dut.ip_out, 0x1001);
}
