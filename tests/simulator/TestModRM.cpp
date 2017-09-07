#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "Fifo.h"
#include "TestModRM.h"
#include "../common/TestModRM.cpp"

class SimulationModRMTestBench : public ModRMDecoderTestBench
{
public:
    SimulationModRMTestBench() : fifo(16)
    {
        decoder = std::make_unique<ModRMDecoder>(
            [&] { return this->fifo.pop(); }, &regs);
    }
    void set_width(OperandWidth width) { decoder->set_width(width); }
    void set_instruction(const std::vector<uint8_t> instruction)
    {
        for (auto &b : instruction)
            fifo.push(b);
    }
    void decode() { decoder->decode(); }
    OperandType get_rm_type() const { return decoder->rm_type(); }
    uint16_t get_effective_address() const
    {
        return decoder->effective_address();
    }
    GPR get_register() const { return decoder->reg(); }
    GPR get_rm_register() const { return decoder->rm_reg(); }

private:
    Fifo<uint8_t> fifo;
    std::unique_ptr<ModRMDecoder> decoder;
};

typedef ::testing::Types<SimulationModRMTestBench> ModRMImplTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Simulation, ModRMTestFixture, ModRMImplTypes);
