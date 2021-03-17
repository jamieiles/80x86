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

#include <SDL_mouse.h>

#include "SoftwareCPU.h"
#include "PIC.h"
#include "../bios/bda.h"
#include "PS2.h"

class Mouse : public PS2
{
public:
    explicit Mouse(PIC *pic)
        : PS2(pic, 0xffe0, 7),
          last_x(0),
          last_y(0),
          capture_enabled(SDL_TRUE),
          left_down(false),
          right_down(false),
          active(false)
    {
        SDL_CaptureMouse(SDL_TRUE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    virtual bool read_pops() const
    {
        return true;
    }

    void write8(uint16_t port_num, unsigned offs, uint8_t v)
    {
        if (port_num == 0 && offs == 0) {
            add_byte(0xfa);
            // Self test passed
            if (v == 0xff) {
                add_byte(0xaa);
                add_byte(0x55);
            }
            if (v == 0xf4)
                active = true;
        }
        PS2::write8(port_num, offs, v);
    }

    void toggle_capture()
    {
        capture_enabled = capture_enabled ? SDL_FALSE : SDL_TRUE;

        SDL_CaptureMouse(capture_enabled);
    }

    void update()
    {
        // Avoid FIFO overflow
        if (num_pending_bytes() >= 5)
            return;

        poll_movement();
    }

    void process_event(SDL_Event e)
    {
        uint8_t v = 0x08;

        if (!active)
            return;

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            left_down = true;
        else if (e.type == SDL_MOUSEBUTTONDOWN &&
                 e.button.button == SDL_BUTTON_RIGHT)
            right_down = true;
        else if (e.type == SDL_MOUSEBUTTONUP &&
                 e.button.button == SDL_BUTTON_LEFT)
            left_down = false;
        else if (e.type == SDL_MOUSEBUTTONUP &&
                 e.button.button == SDL_BUTTON_RIGHT)
            right_down = false;

        if (left_down)
            v |= (1 << 0);
        if (right_down)
            v |= (1 << 1);

        // Avoid FIFO overflow
        if (num_pending_bytes() >= 5)
            return;

        add_byte(v);
        for (auto i = 0; i < 2; ++i)
            add_byte(0);
    }

private:
    int last_x, last_y;
    SDL_bool capture_enabled;
    bool left_down, right_down;
    bool active;

    void poll_movement()
    {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        if (mouse_x == last_x && mouse_y == last_y)
            return;

        if (!active)
            return;

        int x_delta = mouse_x - last_x;
        int y_delta = -(mouse_y - last_y);

        uint8_t b = 0x08;
        if (x_delta < 0)
            b |= (1 << 4);
        if (y_delta < 0)
            b |= (1 << 5);
        add_byte(b);
        add_byte(x_delta);
        add_byte(y_delta);

        last_x = mouse_x;
        last_y = mouse_y;
    }

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // clang-format off
        ar & last_x;
        ar & last_y;
        ar & left_down;
        ar & right_down;
        // clang-format on

        static_cast<PS2 *>(this)->serialize(ar, version);
    }
};
