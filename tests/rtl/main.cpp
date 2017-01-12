#include <memory>

#include <gtest/gtest.h>

#include "RTLCPU.h"

std::unique_ptr<CPU> get_cpu()
{
    return std::make_unique<RTLCPU<>>();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
