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

#include <sys/types.h>
#include <stdint.h>

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#define __unused __attribute__((unused))

const size_t MEMORY_SIZE = 1 * 1024 * 1024;
typedef uint32_t phys_addr;

class Memory
{
public:
    Memory();
    template <typename T>
    void write(phys_addr addr, T val);
    template <typename T>
    T read(phys_addr addr) const;
    bool has_written() const;
    void clear_has_written();

private:
    uint8_t mem[MEMORY_SIZE];
    bool written;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & mem;
        ar & written;
        // clang-format on
    }
};

#ifndef MEM_INIT_BYTE_VAL
#define MEM_INIT_BYTE_VAL 0x00
#endif

constexpr uint8_t mem_init_8 = MEM_INIT_BYTE_VAL;
constexpr uint16_t mem_init_16 =
    (static_cast<uint16_t>(MEM_INIT_BYTE_VAL) << 8) |
    static_cast<uint16_t>(MEM_INIT_BYTE_VAL);
