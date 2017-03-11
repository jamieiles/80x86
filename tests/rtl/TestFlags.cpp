#include <gtest/gtest.h>
#include <VFlags.h>

#include "VerilogTestbench.h"
#include "MicrocodeTypes.h"
#include "RegisterFile.h"

class FlagsTestFixture : public VerilogTestbench<VFlags>,
    public ::testing::Test {
public:
    FlagsTestFixture();
};

FlagsTestFixture::FlagsTestFixture()
{
    reset();
}

static int flag_to_update_mask(Flag f)
{
    switch (f) {
    case CF: return 1 << static_cast<int>(UpdateFlags_CF);
    case PF: return 1 << static_cast<int>(UpdateFlags_PF);
    case AF: return 1 << static_cast<int>(UpdateFlags_AF);
    case ZF: return 1 << static_cast<int>(UpdateFlags_ZF);
    case SF: return 1 << static_cast<int>(UpdateFlags_SF);
    case TF: return 1 << static_cast<int>(UpdateFlags_TF);
    case IF: return 1 << static_cast<int>(UpdateFlags_IF);
    case DF: return 1 << static_cast<int>(UpdateFlags_DF);
    case OF: return 1 << static_cast<int>(UpdateFlags_OF);
    default: abort();
    }
}

TEST_F(FlagsTestFixture, update_flags)
{
    ASSERT_EQ(dut.flags_out, FLAGS_STUCK_BITS);

    dut.flags_in = CF | PF | AF | ZF | SF | IF | DF | OF;
    dut.update_flags = 0;
    cycle();

    ASSERT_EQ(dut.flags_out, FLAGS_STUCK_BITS);

    std::vector<Flag> flags = { CF, PF, AF, ZF, SF, TF, IF, DF, OF };
    for (auto f: flags) {
        reset();

        dut.flags_in = f;
        dut.update_flags = flag_to_update_mask(f);
        cycle();

        ASSERT_EQ(dut.flags_out, f | FLAGS_STUCK_BITS);
    }
}
