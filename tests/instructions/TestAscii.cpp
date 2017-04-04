#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

struct AsciiTest {
    uint16_t ax;
    uint16_t ax_expected;
    uint16_t flags;
    uint16_t flags_expected;
};

static const std::vector<AsciiTest> aaa_tests {
    AsciiTest{0, 0, 0, 0},
    AsciiTest{0, 0, CF, 0},
    AsciiTest{0, 0x0106, AF, AF | CF},
    AsciiTest{0x9, 0x9, 0, 0},
    AsciiTest{0xa, 0x0100, 0, AF | CF},
};

TEST_F(EmulateFixture, Aaa)
{
    // aaa
    for (auto &t: aaa_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0x37 });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

static const std::vector<AsciiTest> daa_tests {
    AsciiTest{0x00, 0x00, 0, 0},
    AsciiTest{0x00, 0x06, AF, AF},
    AsciiTest{0x0a, 0x10, AF, AF},
    AsciiTest{0x00, 0x60, CF, CF},
    AsciiTest{0x9a, 0x00, 0, CF | AF},
};

TEST_F(EmulateFixture, Daa)
{
    // daa
    for (auto &t: daa_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0x27 });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

static const std::vector<AsciiTest> aas_tests {
    AsciiTest{0x00, 0x00, 0, 0},
    AsciiTest{0x0a, 0xff04, 0, AF | CF},
    AsciiTest{0x00, 0xff0a, AF, AF | CF},
};

TEST_F(EmulateFixture, Aas)
{
    // aas
    for (auto &t: aas_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0x3f });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

static const std::vector<AsciiTest> das_tests {
    AsciiTest{0x00, 0x00, 0, ZF | PF},
    AsciiTest{0x0a, 0x04, 0, AF},
    AsciiTest{0x00, 0x9a, AF, AF | PF | SF | CF},
    AsciiTest{0x00, 0xa0, CF, CF | PF | SF},
    AsciiTest{0xff, 0x99, 0, AF | CF | PF | SF}
};

TEST_F(EmulateFixture, Das)
{
    // das
    for (auto &t: das_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0x2f });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

static const std::vector<AsciiTest> aam_tests {
    AsciiTest{0x00, 0x00, 0, ZF | PF},
    AsciiTest{0x0a, 0x0100, 0, PF},
    AsciiTest{0x50, 0x0800, 0, PF},
};

TEST_F(EmulateFixture, Aam)
{
    // aam
    for (auto &t: aam_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0xd4, 0x0a });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

static const std::vector<AsciiTest> aam_129_tests {
    AsciiTest{0x80, 0x0080, 0, 0},
};
TEST_F(EmulateFixture, Aam129)
{
    // aam 129
    for (auto &t: aam_129_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0xd4, 129 });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

TEST_F(EmulateFixture, AamSigned)
{
    // aam 1
    write_flags(0);
    write_reg(AX, 0xffff);
    set_instruction({ 0xd4, 0x10 });

    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | PF);
    EXPECT_EQ(read_reg(AX), 0x0f0f);
}

TEST_F(EmulateFixture, AamDivByZero)
{
    // aam 0
    write_flags(0);
    write_reg(AX, 1);
    set_instruction({ 0xd4, 0 });
    write_mem16(VEC_DIVIDE_ERROR + 0, 0x8000, CS);
    write_mem16(VEC_DIVIDE_ERROR + 2, 0xc000, CS);

    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | 0);
    EXPECT_EQ(read_reg(CS), 0xc000);
    EXPECT_EQ(read_reg(IP), 0x8000);
}

static const std::vector<AsciiTest> aad_tests {
    AsciiTest{0x0000, 0x00, 0, ZF | PF},
    AsciiTest{0x0105, 0x0f, 0, PF},
    AsciiTest{0x0909, 0x63, 0, PF},
};

TEST_F(EmulateFixture, Aad)
{
    // aad
    for (auto &t: aad_tests) {
        reset();

        write_flags(t.flags);
        write_reg(AX, t.ax);
        set_instruction({ 0xd5, 0x0a });

        emulate();

        EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.flags_expected);
        EXPECT_EQ(read_reg(AX), t.ax_expected);
    }
}

TEST_F(EmulateFixture, AadSigned)
{
    // aad 0xff
    write_flags(0);
    write_reg(AX, 0xff00);
    set_instruction({ 0xd5, 0xff });

    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | SF);
    EXPECT_EQ(read_reg(AX), 1);
}
