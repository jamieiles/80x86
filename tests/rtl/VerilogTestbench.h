// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <gtest/gtest.h>

#include "VerilogDriver.h"
#include "TestUtils.h"

template <typename T, bool debug_enabled = verilator_debug_enabled>
class VerilogTestbench : public VerilogDriver<T, debug_enabled>
{
public:
    VerilogTestbench() : VerilogDriver<T, debug_enabled>(current_test_name())
    {
    }
};
