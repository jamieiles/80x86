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

#include <memory>
#include <string>
#include <array>

#include "Cursor.h"

class Window;

class Display
{
public:
    Display(unsigned num_rows = 25, unsigned num_cols = 80);
    ~Display();

    void set_cursor(unsigned row, unsigned col);
    void write_char(uint16_t c);
    void refresh(Cursor cursor);

private:
    void load_font();

    unsigned num_rows;
    unsigned num_cols;
    std::unique_ptr<Window> window;
    unsigned row;
    unsigned col;
    std::unique_ptr<uint16_t[]> characters;
    char font_bitmap[256 * 8];
};
