#include <gtest/gtest.h>
#include <VModRMTestbench.h>
#include <deque>
#include <memory>

#include "VerilogTestbench.h"
#include "TestModRM.h"
#include "../common/TestModRM.cpp"

class RTLModRMDecoderTestbench : public VerilogTestbench<VModRMTestbench>,
    public ModRMDecoderTestBench {
public:
    RTLModRMDecoderTestbench();

    void set_instruction(const std::vector<uint8_t> bytes)
    {
        stream.clear();
        stream.insert(stream.end(), bytes.begin(), bytes.end());
    }

    void set_width(OperandWidth width)
    {
        is_8bit = width == OP_WIDTH_8;
    }

    void decode()
    {
        dut.start = 1;
        cycle();
        dut.start = 0;

        for (auto i = 0; i < 1000; ++i) {
            cycle();
            if (!dut.complete) {
                ASSERT_TRUE(dut.busy);
            } else {
                ASSERT_FALSE(dut.busy);
                return;
            }
        }

        FAIL() << "failed to complete decode" << std::endl;
    }

    OperandType get_rm_type() const
    {
        return dut.rm_is_reg ? OP_REG : OP_MEM;
    }

    uint16_t get_effective_address() const
    {
        return dut.effective_address;
    }

    GPR get_register() const
    {
        return is_8bit ? static_cast<GPR>(dut.regnum + static_cast<int>(AL)) :
            static_cast<GPR>(dut.regnum);
    }

    GPR get_rm_register() const
    {
        return is_8bit ? static_cast<GPR>(dut.rm_regnum + static_cast<int>(AL)) :
            static_cast<GPR>(dut.rm_regnum);
    }
private:
    std::deque<uint8_t> stream;
    bool is_8bit;
};

RTLModRMDecoderTestbench::RTLModRMDecoderTestbench()
    : is_8bit(false)
{
    reset();

    dut.start = 0;
    dut.fifo_rd_en = 0;

    periodic(ClockSetup, [&]{
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() > 0) {
            after_n_cycles(0, [&]{
                this->dut.fifo_empty = this->stream.size() == 0;
                this->dut.fifo_rd_data = this->stream.size() > 0 ?
                    this->stream[0] : 0;
                this->stream.pop_front();
            });
        }
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() == 0)
            FAIL() << "fifo underflow" << std::endl;
    });

    periodic(ClockSetup, [&]{
        after_n_cycles(0, [&]{
            this->dut.regs[0] = regs.get(static_cast<GPR>(this->dut.reg_sel[0]));
            this->dut.regs[1] = regs.get(static_cast<GPR>(this->dut.reg_sel[1]));
        });
    });
}

typedef ::testing::Types<RTLModRMDecoderTestbench> ModRMImplTypes;
INSTANTIATE_TYPED_TEST_CASE_P(RTL, ModRMTestFixture, ModRMImplTypes);

class ModRMFixture : public RTLModRMDecoderTestbench,
    public ::testing::Test {
};

TEST_F(ModRMFixture, StartHeldDoesntClearModrm)
{
    set_instruction({ 0xd8, 0xff });

    dut.start = 1;
    cycle();

    after_n_cycles(0, [&]{
        this->dut.start = 1;
        after_n_cycles(1, [&]{
            this->dut.start = 0;
            this->dut.fifo_rd_data = 0;
            after_n_cycles(1, [&]{ this->dut.fifo_rd_data = 0; });
        });
    });
    cycle(3);

    ASSERT_EQ(this->dut.regnum, 0x3);
}

TEST_F(ModRMFixture, BPBaseInfersSS)
{
    set_instruction({ 0x46, 0x00 });

    decode();

    ASSERT_TRUE(dut.bp_as_base);
}

TEST_F(ModRMFixture, BXBaseDoesntInferSS)
{
    set_instruction({ 0x07 });

    decode();

    ASSERT_FALSE(dut.bp_as_base);
}

TEST_F(ModRMFixture, EAHeld)
{
    regs.set(BX, 0x1234);

    set_instruction({ 0x07 });

    decode();

    ASSERT_EQ(dut.effective_address, 0x1234);
    regs.set(BX, 0x0000);
    cycle();
    ASSERT_EQ(dut.effective_address, 0x1234);
}

TEST_F(ModRMFixture, BPAsBase)
{
    for (uint8_t mod = 0; mod < 4; ++mod) {
        for (uint8_t rm = 0; rm < 8; ++rm) {
            reset();

            set_instruction({ static_cast<uint8_t>((mod << 6) | rm),
                              0xff, 0xff  });
            decode();

            if (mod == 0)
                EXPECT_TRUE(this->dut.bp_as_base ==
                            (rm == 2 || rm == 3));
            else if (mod == 1 || mod == 2)
                EXPECT_TRUE(this->dut.bp_as_base ==
                            (rm == 2 || rm == 3 || rm == 6));
            else
                EXPECT_FALSE(this->dut.bp_as_base);
        }
    }
}
