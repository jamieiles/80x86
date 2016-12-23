#include <gtest/gtest.h>
#include <memory>

#include "CPU.h"

std::unique_ptr<CPU> get_cpu()
{
    return std::make_unique<SoftwareCPU>();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
