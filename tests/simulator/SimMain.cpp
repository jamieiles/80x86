#include <gtest/gtest.h>
#include <memory>

#include "CPU.h"

std::unique_ptr<CPU> get_cpu(const std::string &test_name)
{
    return std::make_unique<SoftwareCPU>(test_name);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
