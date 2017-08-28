#pragma once

#include <memory>
#include <string>
#include <array>

class Window;

struct Cursor {
    int row;
    int col;
};

class Display {
public:
    Display(int num_rows=25, int num_cols=80);
    ~Display();

    void set_cursor(int row, int col);
    void write_char(uint16_t c);
    void refresh();

private:
    void load_font();

    int num_rows;
    int num_cols;
    std::unique_ptr<Window> window;
    Cursor cursor;
    std::unique_ptr<uint16_t[]> characters;
    char font_bitmap[256 * 8];
};
