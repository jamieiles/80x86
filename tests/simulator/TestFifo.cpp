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

#include <gtest/gtest.h>

#include "Fifo.h"

TEST(Fifo, empty_fifo_not_full)
{
    Fifo<int> f(3);

    ASSERT_FALSE(f.is_full());
}

TEST(Fifo, push_fills)
{
    Fifo<int> f(3);

    ASSERT_FALSE(f.is_full());

    for (int i = 0; i < 3; ++i)
        f.push(i);

    ASSERT_TRUE(f.is_full());
}

TEST(Fifo, overflow_throws)
{
    Fifo<int> f(1);

    f.push(1);
    ASSERT_THROW(f.push(2), std::overflow_error);
}

TEST(Fifo, underflow_throws)
{
    Fifo<int> f(1);

    ASSERT_THROW(f.pop(), std::underflow_error);
}

TEST(Fifo, push_pop_in_order)
{
    Fifo<int> f(3);

    for (int i = 0; i < 3; ++i)
        f.push(i);
    for (int i = 0; i < 3; ++i)
        ASSERT_EQ(i, f.pop());
}
