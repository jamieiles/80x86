#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

template <typename T>
struct MulTest {
    T v1;
    T v2;
    T expected;
    T expected_hi;
    uint16_t expected_flags;
};

using Mul8Params = std::pair<const std::vector<uint8_t>,
                             const std::vector<struct MulTest<uint8_t>>>;
using Mul16Params = std::pair<const std::vector<uint8_t>,
                              const std::vector<struct MulTest<uint16_t>>>;

static const std::vector<struct MulTest<uint8_t>> mul8_tests = {
    {0, 0, 0, 0x00, 0}, {0xff, 0, 0, 0x00, 0}, {0, 0xff, 0, 0x00, 0},
        {2, 8, 16, 0x00, 0}, {0x80, 2, 0x00, 0x01, CF | OF},
        {0xff, 0xff, 0x01, 0xfe, CF | OF},
};

static const std::vector<struct MulTest<uint8_t>> imul8_tests = {
    {0, 0, 0, 0x00, 0}, {0xff, 0, 0, 0x00, 0}, {0, 0xff, 0, 0x00, 0},
        {2, 8, 16, 0x00, 0}, {0x80, 2, 0x00, 0xff, CF | OF},
        {0xff, 0xff, 0x01, 0x00, 0}, {0xff, 1, 0xff, 0xff, CF | OF},
        {32, 8, 0x00, 0x01, CF | OF},
};

class MulReg8Test : public EmulateFixture,
                    public ::testing::WithParamInterface<Mul8Params>
{
};
TEST_P(MulReg8Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t : params.second) {
        reset();

        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                     std::to_string(static_cast<int>(t.v2)));
        write_flags(0);
        write_reg(AL, t.v1);
        write_reg(BL, t.v2);
        // xMUL bl
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_EQ(read_reg(AH), t.expected_hi);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}
INSTANTIATE_TEST_CASE_P(Mul,
                        MulReg8Test,
                        ::testing::Values(Mul8Params({0xf6, 0xe3},
                                                     mul8_tests)));
INSTANTIATE_TEST_CASE_P(IMul,
                        MulReg8Test,
                        ::testing::Values(Mul8Params({0xf6, 0xeb},
                                                     imul8_tests)));

class MulMem8Test : public EmulateFixture,
                    public ::testing::WithParamInterface<Mul8Params>
{
};
TEST_P(MulMem8Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t : params.second) {
        reset();

        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                     std::to_string(static_cast<int>(t.v2)));
        write_flags(0);
        write_reg(AL, t.v2);
        write_reg(BX, 0x100);
        write_mem8(0x100, t.v1);

        // xMUL byte [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_EQ(read_reg(AH), t.expected_hi);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}
INSTANTIATE_TEST_CASE_P(Mul,
                        MulMem8Test,
                        ::testing::Values(Mul8Params({0xf6, 0x27},
                                                     mul8_tests)));
INSTANTIATE_TEST_CASE_P(IMul,
                        MulMem8Test,
                        ::testing::Values(Mul8Params({0xf6, 0x2f},
                                                     imul8_tests)));

static const std::vector<struct MulTest<uint16_t>> mul16_tests = {
    {0, 0, 0, 0x00, 0}, {0xff, 0, 0, 0x00, 0}, {0, 0xff, 0, 0x00, 0},
        {2, 8, 16, 0x00, 0}, {0x80, 2, 0x100, 0x00, 0},
        {0x8000, 2, 0x0000, 0x0001, CF | OF},
        {0xffff, 0xffff, 0x0001, 0xfffe, CF | OF},
};

static const std::vector<struct MulTest<uint16_t>> imul16_tests = {
    {0, 0, 0, 0x00, 0}, {0xff, 0, 0, 0x00, 0}, {0, 0xff, 0, 0x00, 0},
        {2, 8, 16, 0x00, 0}, {0x8000, 2, 0x0000, 0xffff, CF | OF},
        {0xffff, 0xffff, 0x0001, 0x0000, 0},
        {0xffff, 1, 0xffff, 0xffff, CF | OF},
        {8192, 8, 0x0000, 0x0001, CF | OF},
};

class MulReg16Test : public EmulateFixture,
                     public ::testing::WithParamInterface<Mul16Params>
{
};
TEST_P(MulReg16Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t : params.second) {
        reset();

        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                     std::to_string(static_cast<int>(t.v2)));
        write_flags(0);
        write_reg(AX, t.v1);
        write_reg(BX, t.v2);
        // xMUL bx
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_EQ(read_reg(DX), t.expected_hi);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}
INSTANTIATE_TEST_CASE_P(Mul,
                        MulReg16Test,
                        ::testing::Values(Mul16Params({0xf7, 0xe3},
                                                      mul16_tests)));
INSTANTIATE_TEST_CASE_P(IMul,
                        MulReg16Test,
                        ::testing::Values(Mul16Params({0xf7, 0xeb},
                                                      imul16_tests)));

class MulMem16Test : public EmulateFixture,
                     public ::testing::WithParamInterface<Mul16Params>
{
};
TEST_P(MulMem16Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t : params.second) {
        reset();

        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                     std::to_string(static_cast<int>(t.v2)));
        write_flags(0);
        write_reg(AX, t.v2);
        write_reg(BX, 0x100);
        write_mem16(0x100, t.v1);

        // xMUL word [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_EQ(read_reg(DX), t.expected_hi);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}
INSTANTIATE_TEST_CASE_P(Mul,
                        MulMem16Test,
                        ::testing::Values(Mul16Params({0xf7, 0x27},
                                                      mul16_tests)));
INSTANTIATE_TEST_CASE_P(IMul,
                        MulMem16Test,
                        ::testing::Values(Mul16Params({0xf7, 0x2f},
                                                      imul16_tests)));

template <typename T>
struct MulImmTest {
    uint16_t v1;
    T v2;
    uint16_t expected;
    uint16_t expected_flags;
};

using MulImm8Params = struct MulImmTest<uint8_t>;
using MulImm16Params = struct MulImmTest<uint16_t>;

class MulImmReg8Test : public EmulateFixture,
                       public ::testing::WithParamInterface<MulImm8Params>
{
};
TEST_P(MulImmReg8Test, ResultAndFlags)
{
    auto t = GetParam();

    SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                 std::to_string(static_cast<int>(t.v2)));
    write_flags(0);
    write_reg(BX, t.v1);
    // IMUL ax, bx, IMM8
    auto instr = std::vector<uint8_t>{0x6b, 0xc3};
    instr.push_back(t.v2);
    set_instruction(instr);

    emulate();

    EXPECT_EQ(read_reg(AX), t.expected);
    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | t.expected_flags);
}
INSTANTIATE_TEST_CASE_P(
    IMul,
    MulImmReg8Test,
    ::testing::Values(MulImm8Params{0, 0, 0, 0},
                      MulImm8Params{0xff, 0, 0, 0},
                      MulImm8Params{0, 0xff, 0, 0},
                      MulImm8Params{2, 8, 16, 0},
                      MulImm8Params{0x8000, 2, 0x0000, CF | OF},
                      MulImm8Params{0xffff, 0xff, 0x0001, 0},
                      MulImm8Params{0xffff, 1, 0xffff, CF | OF},
                      MulImm8Params{8192, 8, 0x0000, CF | OF}));

class MulImmMem8Test : public EmulateFixture,
                       public ::testing::WithParamInterface<MulImm8Params>
{
};
TEST_P(MulImmMem8Test, ResultAndFlags)
{
    auto t = GetParam();

    SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                 std::to_string(static_cast<int>(t.v2)));
    write_flags(0);
    write_reg(BX, 0x100);
    write_mem16(0x100, t.v1);
    // IMUL ax, [bx], IMM8
    auto instr = std::vector<uint8_t>{0x6b, 0x07};
    instr.push_back(t.v2);
    set_instruction(instr);

    emulate();

    EXPECT_EQ(read_reg(AX), t.expected);
    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | t.expected_flags);
}
INSTANTIATE_TEST_CASE_P(
    IMul,
    MulImmMem8Test,
    ::testing::Values(MulImm8Params{0, 0, 0, 0},
                      MulImm8Params{0xff, 0, 0, 0},
                      MulImm8Params{0, 0xff, 0, 0},
                      MulImm8Params{2, 8, 16, 0},
                      MulImm8Params{0x8000, 2, 0x0000, CF | OF},
                      MulImm8Params{0xffff, 0xff, 0x0001, 0},
                      MulImm8Params{0xffff, 1, 0xffff, CF | OF},
                      MulImm8Params{8192, 8, 0x0000, CF | OF}));

class MulImmReg16Test : public EmulateFixture,
                        public ::testing::WithParamInterface<MulImm16Params>
{
};
TEST_P(MulImmReg16Test, ResultAndFlags)
{
    auto t = GetParam();

    SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                 std::to_string(static_cast<int>(t.v2)));
    write_flags(0);
    write_reg(BX, t.v1);
    // IMUL ax, bx, IMM16
    auto instr = std::vector<uint8_t>{0x69, 0xc3};
    instr.push_back(t.v2 & 0xff);
    instr.push_back(t.v2 >> 8);
    set_instruction(instr);

    emulate();

    EXPECT_EQ(read_reg(AX), t.expected);
    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | t.expected_flags);
}
INSTANTIATE_TEST_CASE_P(
    IMul,
    MulImmReg16Test,
    ::testing::Values(MulImm16Params{0, 0, 0, 0},
                      MulImm16Params{0xff, 0, 0, 0},
                      MulImm16Params{0, 0xff, 0, 0},
                      MulImm16Params{2, 8, 16, 0},
                      MulImm16Params{0x8000, 2, 0x0000, CF | OF},
                      MulImm16Params{0xffff, 0xffff, 0x0001, 0},
                      MulImm16Params{0xffff, 1, 0xffff, CF | OF},
                      MulImm16Params{8192, 8, 0x0000, CF | OF}));

class MulImmMem16Test : public EmulateFixture,
                        public ::testing::WithParamInterface<MulImm16Params>
{
};
TEST_P(MulImmMem16Test, ResultAndFlags)
{
    auto t = GetParam();

    SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " * " +
                 std::to_string(static_cast<int>(t.v2)));
    write_flags(0);
    write_reg(BX, 0x100);
    write_mem16(0x100, t.v1);
    // IMUL ax, [bx], IMM16
    auto instr = std::vector<uint8_t>{0x69, 0x07};
    instr.push_back(t.v2 & 0xff);
    instr.push_back(t.v2 >> 8);
    set_instruction(instr);

    emulate();

    EXPECT_EQ(read_reg(AX), t.expected);
    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | t.expected_flags);
}
INSTANTIATE_TEST_CASE_P(
    IMul,
    MulImmMem16Test,
    ::testing::Values(MulImm16Params{0, 0, 0, 0},
                      MulImm16Params{0xff, 0, 0, 0},
                      MulImm16Params{0, 0xff, 0, 0},
                      MulImm16Params{2, 8, 16, 0},
                      MulImm16Params{0x8000, 2, 0x0000, CF | OF},
                      MulImm16Params{0xffff, 0xffff, 0x0001, 0},
                      MulImm16Params{0xffff, 1, 0xffff, CF | OF},
                      MulImm16Params{8192, 8, 0x0000, CF | OF}));
