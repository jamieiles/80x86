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

#include <stdint.h>
#include "Simulator.h"

#include <SDL_events.h>

#include <boost/serialization/list.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include <map>
#include <deque>
#include <set>

#include "SoftwareCPU.h"
#include "PIC.h"
#include "../bios/bda.h"

static const uint8_t ps2_ctrl_clear = 1 << 7;

class PS2 : public IOPorts
{
public:
    PS2(PIC *pic, unsigned short base, int irq_num)
        : IOPorts(base, 1), pic(pic), irq_num(irq_num)
    {
    }

    virtual bool read_pops() const
    {
        return false;
    }

    virtual void write8(uint16_t __unused port_num,
                        unsigned __unused offs,
                        uint8_t __unused v)
    {
        if (offs == 1 && (v & ps2_ctrl_clear))
            if (pending.size())
                pending.clear();
    }

    virtual uint8_t read8(uint16_t __unused port_num, unsigned __unused offs)
    {
        if (offs)
            return pending.size() ? (1 << 0) : 0;

        if (pending.size()) {
            auto v = pending.front();
            if (read_pops())
                pending.pop_front();
            return v;
        } else {
            return 0;
        }
    }

    void add_byte(uint8_t v)
    {
        pic->raise_irq(irq_num);

        pending.push_back(v);
    }

    size_t num_pending_bytes() const
    {
        return pending.size();
    }

private:
    PIC *pic;
    int irq_num;
    std::deque<uint8_t> pending;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & pending;
        // clang-format on
    }
};
