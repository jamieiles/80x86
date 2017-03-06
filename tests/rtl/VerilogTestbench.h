#pragma once

#include <gtest/gtest.h>

#include "VerilogDriver.h"
#include "TestUtils.h"

template <typename T, bool debug_enabled=verilator_debug_enabled>
class VerilogTestbench : public VerilogDriver<T, debug_enabled> {
public:
    VerilogTestbench()
        : VerilogDriver<T, debug_enabled>(current_test_name())
    {
    }
};
