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

#include <cassert>
#include <iostream>

#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "CPU.h"

class PIC : public IOPorts
{
private:
    enum State { STATE_IDLE, STATE_ICW2, STATE_ICW4 };

    enum CommandReadMode { STATE_READ_IRR, STATE_READ_ISR };

public:
    explicit PIC(SimCPU *cpu)
        : IOPorts(0x0020, 1),
          cpu(cpu),
          state(STATE_IDLE),
          command_read_mode(STATE_READ_IRR),
          mask(0),
          isr(0),
          irr(0)
    {
        cpu->set_inta_handler(
            [this](int irq_num) { this->inta_handler(irq_num); });
    }

    void write8(uint16_t __unused port_num, unsigned offs, uint8_t v)
    {
        if (offs == 0)
            write_command(v);
        else
            write_data(v);
    }

    uint8_t read8(uint16_t __unused port_num, unsigned offs)
    {
        if (offs == 0)
            return read_command();
        else
            return read_data();
    }

    void raise_irq(int irq_num)
    {
        if (!(mask & (1 << irq_num)))
            return;

        irr |= (1 << irq_num);

        for (int i = 0; i < 8; ++i) {
            if ((1 << i) & isr)
                return;

            if ((1 << i) & irr) {
                cpu->raise_irq(vector_address + i);
                return;
            }
        }
    }

private:
    void inta_handler(int irq_num)
    {
        isr |= (1 << (irq_num - vector_address));
        irr &= ~(1 << (irq_num - vector_address));
    }

    void write_command(uint8_t v)
    {
        if (is_icw1(v))
            icw1(v);
        else if (is_ocw2(v))
            ocw2(v);
        else if (is_ocw3(v))
            ocw3(v);
    }

    uint8_t read_command()
    {
        if (command_read_mode == STATE_READ_IRR)
            return irr;
        else if (command_read_mode == STATE_READ_ISR)
            return isr;
        return 0;
    }

    uint8_t read_data()
    {
        return mask;
    }

    void write_data(uint8_t v)
    {
        switch (state) {
        case STATE_ICW2:
            vector_address = v & 0xf8;
            state = STATE_ICW4;
            return;
        case STATE_ICW4:
            assert(v == 0x01);
            state = STATE_IDLE;
            return;
        default: set_mask(v);
        }
    }

    bool is_icw1(uint8_t v)
    {
        return v & (1 << 4);
    }

    bool is_ocw2(uint8_t v)
    {
        return (v & (3 << 3)) == 0;
    }

    bool is_ocw3(uint8_t v)
    {
        return (v & (0x80 | (3 << 3))) == 0x08;
    }

    void icw1(uint8_t __unused v)
    {
        state = STATE_ICW2;
        // Only single PIC in edge triggered, 8086 mode supported
        assert((v & 0x0f) == 0x3);
    }

    void ocw2(uint8_t v)
    {
        if (v >> 5 == 1) {
            for (int i = 0; i < 8; ++i) {
                if (isr & (1 << i)) {
                    isr &= ~(1 << i);
                    return;
                }
            }
        } else if (v >> 5 == 3) {
            isr &= ~(1 << (v & 0x7));
        }
    }

    void ocw3(uint8_t v)
    {
        // No poll supported
        assert(!(v & 0x04));
        // No special mask
        assert(!(v & 0x60));

        if ((v & 0x3) == 2)
            command_read_mode = STATE_READ_IRR;
        else if ((v & 0x3) == 3)
            command_read_mode = STATE_READ_ISR;
    }

    void set_mask(uint8_t v)
    {
        mask = v;
    }

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & state;
        ar & command_read_mode;
        ar & vector_address;
        ar & mask;
        ar & isr;
        ar & irr;
        // clang-format on
    }

    SimCPU *cpu;
    State state;
    CommandReadMode command_read_mode;
    uint8_t vector_address;
    uint8_t mask;
    uint8_t isr;
    uint8_t irr;
};
