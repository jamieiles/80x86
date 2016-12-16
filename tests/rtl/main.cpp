#include <gtest/gtest.h>
#include <verilated_cov.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();
#ifdef VM_COVERAGE
    VerilatedCov::write("coverage/rtl-unittest.dat");
#endif

    return ret;
}
