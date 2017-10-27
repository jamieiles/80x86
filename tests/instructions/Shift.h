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

#pragma once

#include <vector>
#include <gtest/gtest.h>
#include "EmulateFixture.h"

template <typename T>
struct ShiftTest {
    int shift_count;
    T val;
    T expected;
    uint16_t flags;
    uint16_t expected_flags;
};

using Shift8Params = std::pair<const std::vector<uint8_t>,
                               const std::vector<struct ShiftTest<uint8_t>>>;
using Shift16Params = std::pair<const std::vector<uint8_t>,
                                const std::vector<struct ShiftTest<uint16_t>>>;

class ShiftReg8Test : public EmulateFixture,
                      public ::testing::WithParamInterface<Shift8Params>
{
};

class ShiftMem8Test : public EmulateFixture,
                      public ::testing::WithParamInterface<Shift8Params>
{
};

class ShiftRegImm8Test : public EmulateFixture,
                         public ::testing::WithParamInterface<Shift8Params>
{
};

class ShiftMemImm8Test : public EmulateFixture,
                         public ::testing::WithParamInterface<Shift8Params>
{
};

class ShiftReg16Test : public EmulateFixture,
                       public ::testing::WithParamInterface<Shift16Params>
{
};

class ShiftMem16Test : public EmulateFixture,
                       public ::testing::WithParamInterface<Shift16Params>
{
};

class ShiftRegImm16Test : public EmulateFixture,
                          public ::testing::WithParamInterface<Shift16Params>
{
};

class ShiftMemImm16Test : public EmulateFixture,
                          public ::testing::WithParamInterface<Shift16Params>
{
};

struct ShiftCLTestParams {
    uint8_t count;
    uint8_t expected;
    std::vector<uint8_t> instruction;
};

class ShiftCLTest : public EmulateFixture,
                    public ::testing::WithParamInterface<ShiftCLTestParams>
{
};
