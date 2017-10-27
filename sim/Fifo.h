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

#include <stdexcept>
#include <vector>

template <typename T>
class Fifo
{
public:
    explicit Fifo(unsigned int max_depth) : max_depth(max_depth)
    {
    }

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
