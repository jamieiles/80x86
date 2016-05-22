#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, ScasbInc)
{
    write_reg(AL, 0);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);
    write_cstring(0x800, "8086");

    // repne scasb
    set_instruction({ 0xf2, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x805);
}

TEST_F(EmulateFixture, ScasbDec)
{
    write_flags(DF);
    write_reg(AL, '1');
    write_reg(DI, 0x803);
    write_reg(CX, 0xff);
    write_cstring(0x800, "1234");

    // repne scasb
    set_instruction({ 0xf2, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7ff);
}

TEST_F(EmulateFixture, ScasbIncRepe)
{
    write_reg(AL, 'a');
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);
    write_cstring(0x800, "aaab");

    // repe scasb
    set_instruction({ 0xf3, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x804);
}

TEST_F(EmulateFixture, ScasbDecRepe)
{
    write_flags(DF);
    write_reg(AL, 'b');
    write_reg(DI, 0x803);
    write_reg(CX, 0xff);
    write_cstring(0x800, "abbb");

    // repe scasb
    set_instruction({ 0xf3, 0xae });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7ff);
}

TEST_F(EmulateFixture, ScaswInc)
{
    write_reg(AX, 0);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);

    for (int i = 0; i < 2; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repne scasw
    set_instruction({ 0xf2, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x806);
}

TEST_F(EmulateFixture, ScaswDec)
{
    write_flags(DF);
    write_reg(AX, 0);
    write_reg(DI, 0x806);
    write_reg(CX, 0xff);

    for (int i = 0; i < 4; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repne scasw
    set_instruction({ 0xf2, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7fc);
}

TEST_F(EmulateFixture, ScaswIncRepe)
{
    write_reg(AX, 0xaa55);
    write_reg(DI, 0x800);
    write_reg(CX, 0xff);

    for (int i = 0; i < 4; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repe scasw
    set_instruction({ 0xf3, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x80a);
}

TEST_F(EmulateFixture, ScaswDecRepe)
{
    write_flags(DF);
    write_reg(AX, 0xaa55);
    write_reg(DI, 0x806);
    write_reg(CX, 0xff);

    for (int i = 0; i < 4; ++i)
        write_mem<uint16_t>(0x800 + i * 2, 0xaa55);

    // repe scasw
    set_instruction({ 0xf3, 0xaf });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x7fc);
}

TEST_F(EmulateFixture, MovsbInc)
{
    write_reg(AL, 0);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 0x4);
    write_cstring(0x800, "8086");

    // repne movsb
    set_instruction({ 0xf3, 0xa4 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x404);
    ASSERT_EQ(read_reg(SI), 0x804);
    ASSERT_EQ(read_cstring(0x400), "8086");
}

TEST_F(EmulateFixture, MovsbDec)
{
    write_flags(DF);
    write_reg(AL, 0);
    write_reg(SI, 0x803);
    write_reg(DI, 0x403);
    write_reg(CX, 0x4);
    write_cstring(0x800, "8086");

    // repne movsb
    set_instruction({ 0xf3, 0xa4 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x3ff);
    ASSERT_EQ(read_reg(SI), 0x7ff);
    ASSERT_EQ(read_cstring(0x400), "8086");
}

TEST_F(EmulateFixture, MovswInc)
{
    write_reg(AL, 0);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, 0x2);
    write_mem<uint16_t>(0x800, 0xaa55);
    write_mem<uint16_t>(0x802, 0x55aa);

    // repne movsw
    set_instruction({ 0xf3, 0xa5 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x404);
    ASSERT_EQ(read_reg(SI), 0x804);
    ASSERT_EQ(read_mem<uint16_t>(0x400), 0xaa55);
    ASSERT_EQ(read_mem<uint16_t>(0x402), 0x55aa);
}

TEST_F(EmulateFixture, MovswDec)
{
    write_flags(DF);
    write_reg(AL, 0);
    write_reg(SI, 0x802);
    write_reg(DI, 0x402);
    write_reg(CX, 0x2);
    write_mem<uint16_t>(0x800, 0xaa55);
    write_mem<uint16_t>(0x802, 0x55aa);

    // repne movsw
    set_instruction({ 0xf3, 0xa5 });

    emulate();

    ASSERT_EQ(read_reg(DI), 0x3fe);
    ASSERT_EQ(read_reg(SI), 0x7fe);
    ASSERT_EQ(read_mem<uint16_t>(0x400), 0xaa55);
    ASSERT_EQ(read_mem<uint16_t>(0x402), 0x55aa);
}

template <typename T>
struct CmpsTest {
    std::vector<T> src;
    std::vector<T> dst;
    uint16_t expected_flags;
    uint8_t prefix;
    uint8_t expected_length;
};

using Cmps8Test = struct CmpsTest<uint8_t>;
using Cmps16Test = struct CmpsTest<uint16_t>;

class Cmps8Fixture : public EmulateFixture,
    public ::testing::WithParamInterface<Cmps8Test> {
};
TEST_P(Cmps8Fixture, Flags)
{
    auto p = GetParam();

    write_flags(0);

    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, std::max(p.src.size() + 1, p.dst.size() + 1));

    write_vector(0x800, p.src);
    write_vector(0x400, p.dst);

    set_instruction({ p.prefix, 0xa6 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | p.expected_flags);
    ASSERT_EQ(p.expected_length, read_reg(SI) - 0x800);
}
INSTANTIATE_TEST_CASE_P(CmpsRepe, Cmps8Fixture,
    ::testing::Values(
        Cmps8Test{std::vector<uint8_t>{'f', 'o', 'o'},
                  std::vector<uint8_t>{'f', 'o', 'o'}, ZF | PF, 0xf3, 4},
        Cmps8Test{std::vector<uint8_t>{'a'},
                  std::vector<uint8_t>{'b'}, SF | PF | CF | AF, 0xf3, 1},
        Cmps8Test{std::vector<uint8_t>{'b'},
                  std::vector<uint8_t>{'a'}, 0, 0xf3, 1},
        Cmps8Test{std::vector<uint8_t>{'b'},
                  std::vector<uint8_t>{}, 0, 0xf3, 1}
    ));
INSTANTIATE_TEST_CASE_P(CmpsRepne, Cmps8Fixture,
    ::testing::Values(
        Cmps8Test{std::vector<uint8_t>{'b', 'a', 'r'},
                  std::vector<uint8_t>{'b', 'a', 'r'}, ZF | PF, 0xf2, 1},
        Cmps8Test{std::vector<uint8_t>{'b', 'a', 'r'},
                  std::vector<uint8_t>{'f', 'o', 'o'}, ZF | PF, 0xf2, 4},
        Cmps8Test{std::vector<uint8_t>{'b', 'a', 'r'},
                  std::vector<uint8_t>{'f', 'o', 'r'}, ZF | PF, 0xf2, 3},
        Cmps8Test{std::vector<uint8_t>{'b', 'a', 'r'},
                  std::vector<uint8_t>{'c'}, ZF | PF, 0xf2, 4}
    ));

TEST_F(EmulateFixture, CmpsbDec)
{
    write_flags(DF);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);

    write_mem<uint8_t>(0x800, 'f');
    write_mem<uint8_t>(0x400, 'g');

    set_instruction({ 0xa6 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | SF | PF | CF | DF | AF);
    ASSERT_EQ(read_reg(SI), 0x7ff);
    ASSERT_EQ(read_reg(DI), 0x3ff);
}

class Cmps16Fixture : public EmulateFixture,
    public ::testing::WithParamInterface<Cmps16Test> {
};
TEST_P(Cmps16Fixture, Flags)
{
    auto p = GetParam();

    write_flags(0);

    write_reg(SI, 0x800);
    write_reg(DI, 0x400);
    write_reg(CX, std::max(p.src.size() + 1, p.dst.size() + 1));

    write_vector(0x800, p.src);
    write_vector(0x400, p.dst);

    set_instruction({ p.prefix, 0xa7 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | p.expected_flags);
    ASSERT_EQ(p.expected_length, read_reg(SI) - 0x800);
}
INSTANTIATE_TEST_CASE_P(CmpsRepe, Cmps16Fixture,
    ::testing::Values(
        Cmps16Test{std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57},
                  std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57}, ZF | PF, 0xf3, 8},
        Cmps16Test{std::vector<uint16_t>{0x1000},
                  std::vector<uint16_t>{0x2000}, SF | PF | CF, 0xf3, 2},
        Cmps16Test{std::vector<uint16_t>{0x2000},
                  std::vector<uint16_t>{0x1000}, PF, 0xf3, 2},
        Cmps16Test{std::vector<uint16_t>{0x2000},
                  std::vector<uint16_t>{}, PF, 0xf3, 2}
    ));
INSTANTIATE_TEST_CASE_P(CmpsRepne, Cmps16Fixture,
    ::testing::Values(
        Cmps16Test{std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57},
                  std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57}, ZF | PF, 0xf2, 2},
        Cmps16Test{std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57},
                  std::vector<uint16_t>{0x55aa, 0x55ab, 0x55ac}, ZF | PF, 0xf2, 6},
        Cmps16Test{std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57},
                  std::vector<uint16_t>{0x55aa, 0x55aa, 0xaa57}, ZF | PF, 0xf2, 4},
        Cmps16Test{std::vector<uint16_t>{0xaa55, 0xaa56, 0xaa57},
                  std::vector<uint16_t>{0x1234}, ZF | PF, 0xf2, 6}
    ));

TEST_F(EmulateFixture, CmpswDec)
{
    write_flags(DF);
    write_reg(SI, 0x800);
    write_reg(DI, 0x400);

    write_mem<uint16_t>(0x800, 0xaa55);
    write_mem<uint16_t>(0x400, 0xaa56);

    set_instruction({ 0xa7 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | SF | PF | CF | DF | AF);
    ASSERT_EQ(read_reg(SI), 0x7fe);
    ASSERT_EQ(read_reg(DI), 0x3fe);
}
