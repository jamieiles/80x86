#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "RTLCPU.h"

std::unique_ptr<CPU> get_cpu(const std::string &test_name)
{
    return std::make_unique<RTLCPU<>>(test_name);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) += "\
-Div/DivMem16Test.*:\
Div/DivMem8Test.*:\
Div/DivReg16Test.*:\
Div/DivReg8Test.*:\
EmulateFixture.Aad:\
EmulateFixture.AadSigned:\
EmulateFixture.Aam:\
EmulateFixture.Aam129:\
EmulateFixture.AamDivByZero:\
EmulateFixture.AamSigned:\
EmulateFixture.Hlt:\
EmulateFixture.Lock:\
IMul/MulMem16Test.*:\
IMul/MulMem8Test.*:\
IMul/MulReg16Test.*:\
IMul/MulReg8Test.*:\
Mul/MulMem16Test.*:\
Mul/MulMem8Test.*:\
Mul/MulReg16Test.*:\
Mul/MulReg8Test.*";

    return RUN_ALL_TESTS();
}
