#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class Fifo {
public:
    explicit Fifo(unsigned int max_depth)
        : max_depth(max_depth)
    {}

    T pop()
    {
        if (entries.size() == 0)
            throw std::underflow_error("Fifo underflow");

        T e = *entries.begin();
        entries.erase(entries.begin());

        return e;
    }

    void push(T e)
    {
        if (is_full())
            throw std::overflow_error("Fifo overflow");
        entries.push_back(e);
    }

    bool is_full() const
    {
        return entries.size() == max_depth;
    }
private:
    const unsigned int max_depth;
    std::vector<T> entries;
};
