#ifndef __ARITHMETIC_H__
#define __ARITHMETIC_H__

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

using Arith8Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct ArithmeticTest<uint8_t>>>;
using Arith16Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct ArithmeticTest<uint16_t>>>;

template <typename T>
struct ArithmeticImmediateTest {
    T v1;
    T expected;
    uint16_t expected_flags;
    bool carry_set;
};

using ArithImmed8Params = std::pair<const std::vector<uint8_t>,
      const struct ArithmeticImmediateTest<uint8_t>>;
using ArithImmed16Params = std::pair<const std::vector<uint8_t>,
      const struct ArithmeticImmediateTest<uint16_t>>;

class ArithmeticRegReg8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Arith8Params> {
};

class ArithmeticMemReg8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Arith8Params> {
};

class ArithmeticRegReg8TestReversed : public EmulateFixture,
    public ::testing::WithParamInterface<Arith8Params> {
};

class ArithmeticMemReg8TestReversed : public EmulateFixture,
    public ::testing::WithParamInterface<Arith8Params> {
};

class ArithmeticRegReg16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Arith16Params> {
};

class ArithmeticRegMem16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Arith16Params> {
};

class ArithmeticRegReg16TestReversed : public EmulateFixture,
    public ::testing::WithParamInterface<Arith16Params> {
};

class ArithmeticMemReg16TestReversed : public EmulateFixture,
    public ::testing::WithParamInterface<Arith16Params> {
};

class ArithmeticRegImmed8Test : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed8Params> {
};

class ArithmeticMemImmed8Test : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed8Params> {
};

class ArithmeticRegImmed16Test : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed16Params> {
};

class ArithmeticMemImmed16Test : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed16Params> {
};

class ArithmeticRegImmed16TestExtend : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed16Params> {
};

class ArithmeticMemImmed16TestExtend : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed16Params> {
};

class ArithmeticAlImmedTest : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed8Params> {
};

class ArithmeticAxImmedTest : public EmulateFixture,
    public ::testing::WithParamInterface<ArithImmed16Params> {
};

#endif /* __ARITHMETIC_H__ */
