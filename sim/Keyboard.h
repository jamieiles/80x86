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
#include "PS2.h"

static std::map<int, std::vector<unsigned char>> sdl_to_keyboard = {
    {SDLK_a, {0x1e}},
    {SDLK_b, {0x30}},
    {SDLK_c, {0x2e}},
    {SDLK_d, {0x20}},
    {SDLK_e, {0x12}},
    {SDLK_f, {0x21}},
    {SDLK_g, {0x22}},
    {SDLK_h, {0x23}},
    {SDLK_i, {0x17}},
    {SDLK_j, {0x24}},
    {SDLK_k, {0x25}},
    {SDLK_l, {0x26}},
    {SDLK_m, {0x32}},
    {SDLK_n, {0x31}},
    {SDLK_o, {0x18}},
    {SDLK_p, {0x19}},
    {SDLK_q, {0x10}},
    {SDLK_r, {0x13}},
    {SDLK_s, {0x1F}},
    {SDLK_t, {0x14}},
    {SDLK_u, {0x16}},
    {SDLK_v, {0x2F}},
    {SDLK_w, {0x11}},
    {SDLK_x, {0x2D}},
    {SDLK_y, {0x15}},
    {SDLK_z, {0x2C}},
    {SDLK_1, {0x02}},
    {SDLK_2, {0x03}},
    {SDLK_3, {0x04}},
    {SDLK_4, {0x05}},
    {SDLK_5, {0x06}},
    {SDLK_6, {0x07}},
    {SDLK_7, {0x08}},
    {SDLK_8, {0x09}},
    {SDLK_9, {0x0A}},
    {SDLK_0, {0x0B}},
    {SDLK_MINUS, {0x0c}},
    {SDLK_EQUALS, {0x0d}},
    {SDLK_LEFTBRACKET, {0x1a}},
    {SDLK_RIGHTBRACKET, {0x1b}},
    {SDLK_SEMICOLON, {0x27}},
    {SDLK_QUOTE, {0x28}},
    {SDLK_BACKQUOTE, {0x20}},
    {SDLK_BACKSLASH, {0x2b}},
    {SDLK_COMMA, {0x33}},
    {SDLK_PERIOD, {0x34}},
    {SDLK_SLASH, {0x35}},
    {SDLK_F1, {0x3b}},
    {SDLK_F2, {0x3c}},
    {SDLK_F3, {0x3d}},
    {SDLK_F4, {0x3e}},
    {SDLK_F5, {0x3f}},
    {SDLK_F6, {0x40}},
    {SDLK_F7, {0x41}},
    {SDLK_F8, {0x42}},
    {SDLK_F9, {0x43}},
    {SDLK_F10, {0x44}},
    {SDLK_F11, {0x85}},
    {SDLK_F12, {0x86}},
    {SDLK_BACKSPACE, {0x0e}},
    {SDLK_DELETE, {0xe0, 0x53}},
    {SDLK_DOWN, {0xe0, 0x50}},
    {SDLK_END, {0xe0, 0x4f}},
    {SDLK_RETURN, {0x1c}},
    {SDLK_ESCAPE, {0x01}},
    {SDLK_HOME, {0xe0, 0x47}},
    {SDLK_INSERT, {0xe0, 0x52}},
    {SDLK_LEFT, {0xe0, 0x4b}},
    {SDLK_PAGEDOWN, {0xe0, 0x51}},
    {SDLK_PAGEUP, {0xe0, 0x49}},
    {SDLK_PRINTSCREEN, {0xe0, 0x12, 0xe0, 0x7c}},
    {SDLK_RIGHT, {0xe0, 0x4d}},
    {SDLK_SPACE, {0x39}},
    {SDLK_TAB, {0x0f}},
    {SDLK_UP, {0xe0, 0x48}},
    {SDLK_LCTRL, {0x1d}},
    {SDLK_LALT, {0x38}},
    {SDLK_LSHIFT, {0x2a}},
};

class Keyboard : public PS2
{
public:
    explicit Keyboard(PIC *pic) : PS2(pic, 0x0060, 1)
    {
        add_byte(0xaa);
    }

    void process_event(SDL_Event e)
    {
        if (sdl_to_keyboard.count(e.key.keysym.sym) == 0)
            return;

        for (auto b : sdl_to_keyboard[e.key.keysym.sym]) {
            if (b == 0xe0)
                continue;
            else
                add_byte(e.type == SDL_KEYUP ? 0x80 | b : b);
        }
    }
};
