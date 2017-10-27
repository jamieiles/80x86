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

#include <deque>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdexcept>

#include <boost/serialization/list.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "UART.h"
#include "CPU.h"

class RawTTY
{
public:
    RawTTY()
    {
        if (tcgetattr(STDIN_FILENO, &old_termios))
            throw std::runtime_error("Failed to get termios");

        auto termios = old_termios;
        cfmakeraw(&termios);
        termios.c_cc[VMIN] = 0;
        termios.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &termios))
            throw std::runtime_error("Failed to set new termios");
    }

    ~RawTTY()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    }

private:
    struct termios old_termios;
};

class UART : public IOPorts
{
public:
    UART();
    void write8(uint16_t port_num, unsigned offs, uint8_t v);
    void write16(uint16_t port_num, uint16_t v);
    uint8_t read8(uint16_t port_num, unsigned offs);
    uint16_t read16(uint16_t port_num);
    void add_char(int c);

private:
    RawTTY raw_tty;
    std::deque<uint8_t> charbuf;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & charbuf;
        // clang-format on
    }
};
