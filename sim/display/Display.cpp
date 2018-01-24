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

#include "Display.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

#include <SDL.h>

#include "Window.h"

Display::Display(unsigned num_rows, unsigned num_cols)
    : num_rows(num_rows), num_cols(num_cols), row(0), col(0), is_graphics(false)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error("Failed to initialize SDL " +
                                 std::string(SDL_GetError()));

    characters = std::make_unique<uint16_t[]>(num_rows * (num_cols + 1));
    window = std::make_unique<Window>("8086sim", 8 * num_cols, 8 * num_rows);

    load_font();
}

Display::~Display()
{
    SDL_Quit();
}

void Display::load_font()
{
    std::ifstream font_file;
    font_file.open(FONT_FILE, std::ios::binary | std::ios::in);
    font_file.read(font_bitmap, sizeof(font_bitmap));
    font_file.close();
}

void Display::set_cursor(unsigned row, unsigned col)
{
    this->row = row;
    this->col = col;
}

void Display::write_char(uint16_t c)
{
    uint16_t v = c;

    characters[this->row * (num_cols + 1) + this->col] = v;
    this->col++;

    if (this->col == num_cols) {
        this->col = 0;
        this->row++;
    }
    if (this->row == num_rows)
        this->row = num_rows - 1;
}

struct color cga_full_palette[16] = {
    {0, 0, 0},          // black
    {0, 0, 0xaa},       // blue
    {0x00, 0xaa, 0x00}, // green
    {0x00, 0xaa, 0xaa}, // cyan
    {0xaa, 0x00, 0x00}, // red
    {0xaa, 0x00, 0xaa}, // magenta
    {0xaa, 0x55, 0x00}, // brown
    {0xaa, 0xaa, 0xaa}, // white
    {0x55, 0x55, 0x55}, // gray
    {0x55, 0x55, 0xff}, // bright blue
    {0x55, 0xff, 0x55}, // bright green
    {0x55, 0xff, 0xff}, // bright cyan
    {0xff, 0x55, 0x55}, // bright red
    {0xff, 0x55, 0xff}, // bright magenta
    {0xff, 0xff, 0x55}, // yellow
    {0xff, 0xff, 0xff}, // bright white
};

const struct color get_color(unsigned char idx)
{
    assert(idx < 16);

    return cga_full_palette[idx];
}

struct color Display::get_graphics_color(unsigned char idx) const
{
    assert(idx < 4);

    return graphics_palette.colors[idx];
}

void Display::refresh()
{
    assert(is_graphics);
    window->clear();

    for (unsigned y = 0; y < 200; ++y)
        for (unsigned x = 0; x < 320; ++x) {
            auto color = get_graphics_color(pixels[y][x]);

            for (int i = 0; i < 2; ++i)
                window->set_pixel(x * 2 + i, y, color.r, color.g, color.b);
        }

    window->redraw();

    SDL_UpdateWindowSurface(window->get());
}

void Display::refresh(Cursor cursor)
{
    assert(!is_graphics);

    window->clear();
    for (unsigned y = 0; y < num_rows; ++y) {
        for (unsigned x = 0; x < num_cols; ++x) {
            uint16_t font_index = characters.get()[(y * (num_cols + 1)) + x];
            char *font_data = &font_bitmap[(font_index & 0xff) * 8];

            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j) {
                    auto fg = get_color((font_index >> 8) & 0xf);
                    auto bg = get_color((font_index >> 12) & 0xf);

                    auto render_cursor =
                        cursor.is_active_at(y * 8 + i, x * 8 + j);
                    bool pixel_set = (font_data[i] & (1 << (8 - j)));
                    auto color = (pixel_set && !render_cursor) ||
                                         (!pixel_set && render_cursor)
                                     ? fg
                                     : bg;
                    window->set_pixel(x * 8 + j, y * 8 + i, color.r, color.g,
                                      color.b);
                }
        }
    }

    window->redraw();

    SDL_UpdateWindowSurface(window->get());
}
