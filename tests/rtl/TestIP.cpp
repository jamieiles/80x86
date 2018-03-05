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
#include <VIP.h>

#include "VerilogTestbench.h"

class IPTestFixture : public VerilogTestbench<VIP>, public ::testing::Test
{
public:
    void inc();
    uint16_t get();
    void commit();
    void rollback();
    void set(uint16_t v);
};

void IPTestFixture::inc()
{
    this->dut.inc = 1;
    this->dut.start_instruction = 1;
    after_n_cycles(1, [&] {
        this->dut.inc = 0;
        this->dut.start_instruction = 0;
    });
    cycle();
}

uint16_t IPTestFixture::get()
{
    return this->dut.val;
}

void IPTestFixture::set(uint16_t val)
{
    this->dut.wr_val = val;
    this->dut.wr_en = 1;
    after_n_cycles(1, [&] { this->dut.wr_en = 0; });
    cycle();
}

void IPTestFixture::commit()
{
    this->dut.start_instruction = 1;
    after_n_cycles(1, [&] { this->dut.start_instruction = 0; });
    cycle();
}

void IPTestFixture::rollback()
{
    this->dut.rollback = 1;
    after_n_cycles(1, [&] { this->dut.rollback = 0; });
    cycle();
}

TEST_F(IPTestFixture, IncrementAndReset)
{
    ASSERT_EQ(0U, get());
    inc();
    inc();
    ASSERT_EQ(2U, get());

    reset();

    ASSERT_EQ(0U, get());
}

TEST_F(IPTestFixture, SetNewValue)
{
    ASSERT_EQ(0U, get());

    set(0xa55a);

    ASSERT_EQ(0xa55a, get());
}
