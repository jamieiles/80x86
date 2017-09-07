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

Display::Display(int num_rows, int num_cols)
    : num_rows(num_rows), num_cols(num_cols), cursor{0, 0}
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

void Display::set_cursor(int row, int col)
{
    assert(row >= 0 && row < num_rows);
    assert(col >= 0 && col < num_cols);

    cursor.row = row;
    cursor.col = col;
}

void Display::write_char(uint16_t c)
{
    uint16_t v = c;

    characters[cursor.row * (num_cols + 1) + cursor.col] = v;
    cursor.col++;

    if (cursor.col == num_cols) {
        cursor.col = 0;
        cursor.row++;
    }
    if (cursor.row == num_rows)
        cursor.row = num_rows - 1;
}

struct color {
    unsigned char r, g, b;
};

const struct color get_color(unsigned char idx)
{
    assert(idx < 16);

    struct color lut[] = {
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

    return lut[idx];
}

void Display::refresh()
{
    window->clear();
    for (int row = 0; row < num_rows; ++row) {
        for (int col = 0; col < num_cols; ++col) {
            uint16_t font_index =
                characters.get()[(row * (num_cols + 1)) + col];
            char *font_data = &font_bitmap[(font_index & 0xff) * 8];

            for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j) {
                    auto fg = get_color((font_index >> 8) & 0xf);
                    auto bg = get_color((font_index >> 12) & 0xf);

                    if (font_data[i] & (1 << (8 - j)))
                        window->set_pixel(col * 8 + j, row * 8 + i, fg.r, fg.g,
                                          fg.b);
                    else
                        window->set_pixel(col * 8 + j, row * 8 + i, bg.r, bg.g,
                                          bg.b);
                }
        }
    }

    window->redraw();

    SDL_UpdateWindowSurface(window->get());
}
