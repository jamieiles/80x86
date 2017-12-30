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

#include <stdexcept>
#include <VDivider.h>
#include "VerilogTestbench.h"

using DivideError = std::runtime_error;

class DividerTestbench : public VerilogTestbench<VDivider>,
                         public ::testing::Test
{
public:
    DividerTestbench() : done(false), quotient(0), remainder(0)
    {
        reset();

        periodic(ClockCapture, [this] {
            if (this->dut.complete) {
                this->done = true;
                this->quotient = this->dut.quotient;
                this->remainder = this->dut.remainder;
            }
        });
    }
    void divide(uint32_t dividend,
                uint16_t divisor,
                bool is_8_bit = false,
                bool is_signed = false);

protected:
    bool done;
    uint16_t quotient, remainder;
};

void DividerTestbench::divide(uint32_t dividend,
                              uint16_t divisor,
                              bool is_8_bit,
                              bool is_signed)
{
    after_n_cycles(0, [this, dividend, divisor, is_8_bit, is_signed] {
        this->dut.dividend = dividend;
        this->dut.divisor = divisor;
        this->dut.start = 1;
        this->dut.is_8_bit = is_8_bit;
        this->dut.is_signed = is_signed;
        after_n_cycles(1, [this] { this->dut.start = 0; });
    });

    for (int i = 0; i < 100; ++i) {
        cycle();
        if (!done)
            continue;
        if (this->dut.error)
            throw DivideError("error");
        return;
    }

    FAIL() << "Execution timeout";
}

template <typename T1, typename T2>
struct DivTest {
    T1 v1;
    T2 v2;
    T2 quotient;
    T2 remainder;
    bool expect_error;
};

using Div8Params = struct DivTest<uint16_t, uint8_t>;
using Div16Params = struct DivTest<uint32_t, uint16_t>;
using IDiv8Params = struct DivTest<int16_t, int8_t>;
using IDiv16Params = struct DivTest<int32_t, int16_t>;

class DivTests16 : public DividerTestbench,
                   public ::testing::WithParamInterface<Div16Params>
{
};
TEST_P(DivTests16, Result)
{
    auto p = GetParam();

    if (p.expect_error) {
        EXPECT_THROW(divide(p.v1, p.v2, false, false), DivideError);
    } else {
        divide(p.v1, p.v2, false, false);
        EXPECT_EQ(quotient, p.quotient);
        EXPECT_EQ(remainder, p.remainder);
    }

    cycle(3);
}
INSTANTIATE_TEST_CASE_P(
    Div,
    DivTests16,
    ::testing::Values(Div16Params{100, 20, 5, 0, false},
                      Div16Params{500, 250, 2, 0, false},
                      Div16Params{10, 3, 3, 1, false},
                      Div16Params{128000, 10, 12800, 0, false},
                      Div16Params{130000, 32500, 4, 0, false},
                      Div16Params{0xffff, 1, 0xffff, 0, false},
                      // Divide by zero
                      Div16Params{130000, 0, 0, 0, true},
                      // Destination overflow
                      Div16Params{0x10000, 1, 0, 0, true}));

class DivTests8 : public DividerTestbench,
                  public ::testing::WithParamInterface<Div8Params>
{
};
TEST_P(DivTests8, Result)
{
    auto p = GetParam();

    if (p.expect_error) {
        EXPECT_THROW(divide(p.v1, p.v2, true, false), DivideError);
    } else {
        divide(p.v1, p.v2, true);
        EXPECT_EQ(quotient, p.quotient);
        EXPECT_EQ(remainder, p.remainder);
    }

    cycle(3);
}
INSTANTIATE_TEST_CASE_P(Div,
                        DivTests8,
                        ::testing::Values(Div8Params{100, 20, 5, 0, false},
                                          Div8Params{500, 250, 2, 0, false},
                                          Div8Params{10, 3, 3, 1, false},
                                          Div8Params{0xff, 1, 0xff, 0, false},
                                          // Divide by zero
                                          Div8Params{1, 0, 0, 0, true},
                                          // Destination overflow
                                          Div8Params{0x100, 1, 0, 0, true}));

TEST_F(DividerTestbench, MSBsIgnored8Bit)
{
    ASSERT_NO_THROW(divide(0x10008000, 0xff, true, false));

    EXPECT_EQ(quotient & 0xff, 0x80);
    EXPECT_EQ(remainder & 0xff, 0x80);
}

class IDivTests16 : public DividerTestbench,
                    public ::testing::WithParamInterface<IDiv16Params>
{
};
TEST_P(IDivTests16, Result)
{
    auto p = GetParam();

    if (p.expect_error) {
        EXPECT_THROW(divide(p.v1, p.v2, false, true), DivideError);
    } else {
        divide(p.v1, p.v2, false, true);
        EXPECT_EQ(static_cast<int16_t>(quotient), p.quotient);
        EXPECT_EQ(static_cast<int16_t>(remainder), p.remainder);
    }

    cycle(3);
}
INSTANTIATE_TEST_CASE_P(
    IDiv,
    IDivTests16,
    ::testing::Values(IDiv16Params{10, 3, 3, 1, false},
                      IDiv16Params{10, -3, -3, 1, false},
                      IDiv16Params{-10, -3, 3, -1, false},
                      IDiv16Params{-10, 3, -3, -1, false},
                      IDiv16Params{-10, 0, 0, 0, true},
                      IDiv16Params{65534, 2, 32767, 0, false},
                      IDiv16Params{-65536, 2, -32768, 0, false},
                      IDiv16Params{-1, 2, 0, -1, false},
                      // Positive integer overflow
                      IDiv16Params{65536, 2, 0, 0, true},
                      // Negative integer overflow
                      IDiv16Params{-65538, 2, 0, 0, true}));

class IDivTests8 : public DividerTestbench,
                   public ::testing::WithParamInterface<IDiv8Params>
{
};
TEST_P(IDivTests8, Result)
{
    auto p = GetParam();

    if (p.expect_error) {
        EXPECT_THROW(divide(p.v1, p.v2, true, true), DivideError);
    } else {
        divide(p.v1, p.v2, true, true);
        EXPECT_EQ(static_cast<int8_t>(quotient), p.quotient);
        EXPECT_EQ(static_cast<int8_t>(remainder), p.remainder);
    }

    cycle(3);
}
INSTANTIATE_TEST_CASE_P(IDiv,
                        IDivTests8,
                        ::testing::Values(IDiv8Params{10, 3, 3, 1, false},
                                          IDiv8Params{10, -3, -3, 1, false},
                                          IDiv8Params{-10, -3, 3, -1, false},
                                          IDiv8Params{-10, 3, -3, -1, false},
                                          IDiv8Params{-1, 2, 0, -1, false},
                                          // Divide by zero
                                          IDiv8Params{-10, 0, 0, 0, true},
                                          IDiv8Params{254, 2, 127, 0, false},
                                          IDiv8Params{-256, 2, -128, 0, false},
                                          // Positive integer overflow
                                          IDiv8Params{256, 2, 0, 0, true},
                                          // Negative integer overflow
                                          IDiv8Params{-258, 2, 0, 0, true}));
