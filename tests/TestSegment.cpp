#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

// 0x26: es:
// 0x2e: cs:
// 0x36: ss:
// 0x3e: ds:
//
// stack transfers don't get segment override used
// string destination doesn't get segment override used

struct MovOverrideTest {
    const char *segment;
    GPR segment_register;
    uint8_t prefix_byte;
};

class MovOverride : public EmulateFixture,
    public ::testing::WithParamInterface<struct MovOverrideTest> {
};
TEST_P(MovOverride, SegmentOverriden)
{
    auto t = GetParam();

    SCOPED_TRACE(t.segment);

    write_reg(DS, 0x2000);
    write_reg(t.segment_register, 0x8000);
    // mov word [SEGMENT:0x0100], 0xaa55
    set_instruction({ t.prefix_byte, 0xc7, 0x06, 0x00, 0x01, 0x55, 0xaa });

    emulate();

    ASSERT_EQ(0xaa55, read_mem<uint16_t>(get_phys_addr(0x8000, 0x0100)));
    ASSERT_EQ(0x0000, read_mem<uint16_t>(get_phys_addr(0x2000, 0x0100)));
}
INSTANTIATE_TEST_CASE_P(SegmentOverride, MovOverride,
    ::testing::Values(
        MovOverrideTest{"ES", ES, 0x26},
        MovOverrideTest{"CS", CS, 0x2e},
        MovOverrideTest{"SS", SS, 0x36}
    ));

TEST_F(EmulateFixture, DSSegmentOverrideIsNop)
{
    write_reg(DS, 0x8000);
    // mov word [ds:0x0100], 0xaa55
    set_instruction({ 0x3e, 0xc7, 0x06, 0x00, 0x01, 0x55, 0xaa });

    emulate();

    ASSERT_EQ(0xaa55, read_mem<uint16_t>(get_phys_addr(0x8000, 0x0100)));
}

TEST_F(EmulateFixture, PushMemSegmentOverride)
{
    write_reg(SS, 0x4000);
    write_reg(ES, 0x2000);
    write_reg(BX, 0x0100);
    write_reg(SP, 0x0080);

    write_mem<uint16_t>(get_phys_addr(0x2000, 0x0100), 0xaa55);

    // push word [es:bx]
    set_instruction({ 0x26, 0xff, 0x37 });

    emulate();

    ASSERT_EQ(read_reg(SP), 0x007e);
    ASSERT_EQ(read_mem<uint16_t>(get_phys_addr(0x4000, 0x007e)), 0xaa55);
}

TEST_F(EmulateFixture, PopMemSegmentOverride)
{
    write_reg(SS, 0x4000);
    write_reg(ES, 0x2000);
    write_reg(BX, 0x0100);
    write_reg(SP, 0x007e);

    write_mem<uint16_t>(get_phys_addr(0x4000, 0x007e), 0xaa55);

    // pop word [es:bx]
    set_instruction({ 0x26, 0x8f, 0x07 });

    emulate();

    ASSERT_EQ(read_reg(SP), 0x0080);
    ASSERT_EQ(read_mem<uint16_t>(get_phys_addr(0x2000, 0x0100)), 0xaa55);
}
