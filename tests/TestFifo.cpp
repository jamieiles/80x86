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
