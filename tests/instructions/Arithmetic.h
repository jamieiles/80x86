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

#include <gtest/gtest.h>

#include "EmulateFixture.h"

template <typename T>
struct ArithmeticTest {
    T v1;
    T v2;
    T expected;
    uint16_t expected_flags;
    uint16_t carry_set;
};

using Arith8Params =
    std::pair<const std::vector<uint8_t>,
              const std::vector<struct ArithmeticTest<uint8_t>>>;
using Arith16Params =
    std::pair<const std::vector<uint8_t>,
              const std::vector<struct ArithmeticTest<uint16_t>>>;

template <typename T>
struct ArithmeticImmediateTest {
    T v1;
    T expected;
    uint16_t expected_flags;
    bool carry_set;
};

using ArithImmed8Params =
    std::pair<const std::vector<uint8_t>,
              const struct ArithmeticImmediateTest<uint8_t>>;
using ArithImmed16Params =
    std::pair<const std::vector<uint8_t>,
              const struct ArithmeticImmediateTest<uint16_t>>;

class ArithmeticRegReg8Test : public EmulateFixture,
                              public ::testing::WithParamInterface<Arith8Params>
{
};

class ArithmeticMemReg8Test : public EmulateFixture,
                              public ::testing::WithParamInterface<Arith8Params>
{
};

class ArithmeticRegReg8TestReversed
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith8Params>
{
};

class ArithmeticRegReg8TestReversedNoResult
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith8Params>
{
};

class ArithmeticMemReg8TestReversed
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith8Params>
{
};

class ArithmeticMemReg8TestReversedNoResult
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith8Params>
{
};

class ArithmeticRegReg16Test
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith16Params>
{
};

class ArithmeticRegMem16Test
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith16Params>
{
};

class ArithmeticRegReg16TestReversed
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith16Params>
{
};

class ArithmeticMemReg16TestReversed
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith16Params>
{
};

class ArithmeticRegReg16TestReversedNoResult
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith16Params>
{
};

class ArithmeticMemReg16TestReversedNoResult
    : public EmulateFixture,
      public ::testing::WithParamInterface<Arith16Params>
{
};

class ArithmeticRegImmed8Test
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed8Params>
{
};

class ArithmeticMemImmed8Test
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed8Params>
{
};

class ArithmeticRegImmed16Test
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed16Params>
{
};

class ArithmeticMemImmed16Test
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed16Params>
{
};

class ArithmeticRegImmed16TestExtend
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed16Params>
{
};

class ArithmeticMemImmed16TestExtend
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed16Params>
{
};

class ArithmeticAlImmedTest
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed8Params>
{
};

class ArithmeticAxImmedTest
    : public EmulateFixture,
      public ::testing::WithParamInterface<ArithImmed16Params>
{
};
