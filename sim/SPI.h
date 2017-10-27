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
#include "CPU.h"
#include <cassert>
#include <deque>
#include <stdint.h>
#include <fstream>

#include <boost/serialization/list.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

// A dumb SPI mode SD card emulation.  Only supports basic block operations,
// standard capacity, no CRC checking or generation and no real errors, this
// is not a real SD card model!
//
// Blocklen is ignored and will always be 512 bytes.
// ACMD messages are ignored too.
class SPI : public IOPorts
{
private:
    enum SPIState {
        STATE_IDLE,
        STATE_RECEIVING,
        STATE_TRANSMITTING,
        STATE_WAIT_FOR_DATA,
        STATE_DO_WRITE_BLOCK,
    };

public:
    SPI(const std::string &disk_image_path);
    void write8(uint16_t port_num, unsigned offs, uint8_t v);
    void write16(uint16_t port_num, uint16_t v);
    uint8_t read8(uint16_t port_num, unsigned offs);
    uint16_t read16(uint16_t __unused port_num);

private:
    void transfer(uint8_t mosi_val);
    bool transmit_ready();
    void read_block();
    void write_block();
    uint16_t control_reg;
    uint8_t rx_val;
    SPIState state;
    std::vector<uint8_t> mosi_buf;
    std::deque<uint8_t> miso_buf;
    std::fstream disk_image;
    bool do_write;
    unsigned write_count;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & control_reg;
        ar & rx_val;
        ar & state;
        ar & mosi_buf;
        ar & miso_buf;
        ar & do_write;
        ar & write_count;
        // clang-format on
    }
};
