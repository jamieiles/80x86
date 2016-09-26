#include "Display.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

#include <SDL.h>

#include "Font.h"
#include "Window.h"

Display::Display(int num_rows, int num_cols)
    : num_rows(num_rows),
    num_cols(num_cols),
    cursor{0, 0}
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error("Failed to initialize SDL");
    if (TTF_Init() != 0)
        throw std::runtime_error("Failed to initialize SDL TTF");

    // Each row has a NUL terminator
    characters = std::make_unique<char[]>(num_rows * (num_cols + 1));
    font = std::make_unique<Font>("/usr/share/fonts/truetype/droid/DroidSansMono.ttf");
    window = std::make_unique<Window>("8086sim",
                                      num_cols * (font->glyph_width() + 1),
                                      num_rows * (font_size + 1));
}

Display::~Display()
{
    SDL_Quit();
}

void Display::set_cursor(int row, int col)
{
    assert(row >= 0 && row < num_rows);
    assert(col >= 0 && col < num_cols);

    cursor.row = row;
    cursor.col = col;
}

void Display::write_char(char c)
{
    if (c == '\n')
        cursor.row++;
    else if (c == '\r')
        cursor.col = 0;
    else if (isprint(c)) {
        characters[cursor.row * (num_cols + 1) + cursor.col] = c;
        cursor.col++;
    }

    if (cursor.col == num_cols) {
        cursor.col = 0;
        cursor.row++;
    }
    if (cursor.row == num_rows) {
        shift_rows();
        cursor.row = num_rows - 1;
    }
}

void Display::shift_rows()
{
    memmove(characters.get(), characters.get() + num_cols + 1,
            ((num_rows - 1) * (num_cols + 1)));
    memset(characters.get() + (num_rows - 1) * (num_cols + 1),
           '\0', num_cols + 1);
}

void Display::scroll(int scroll_count)
{
    if (scroll_count == 0)
        scroll_count = num_rows;

    for (int i = 0; i < scroll_count; ++i)
        shift_rows();
}

void Display::refresh()
{
    SDL_Color fg{255, 255, 255, 128};

    SDL_FillRect(window->surface(), NULL, 0);

    for (int row = 0; row < num_rows; ++row) {
        auto text_surface = TTF_RenderText_Solid(font->get(),
                                                 characters.get() + row * (num_cols + 1),
                                                 fg);
        SDL_Rect render_pos = {0, row * font_size + 1, window->get_width(),
            window->get_height()};
        SDL_BlitSurface(text_surface, NULL, window->surface(), &render_pos);
        SDL_FreeSurface(text_surface);
    }

    SDL_UpdateWindowSurface(window->get());
}
