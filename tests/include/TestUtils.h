#pragma once

#include <boost/format.hpp>

#include <gtest/gtest.h>

static inline std::string current_test_name()
{
    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    return (boost::format("%s.%s") % test_info->test_case_name() %
            test_info->name()).str();
}
