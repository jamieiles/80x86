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

    void set_cursor(int row, int col);
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
