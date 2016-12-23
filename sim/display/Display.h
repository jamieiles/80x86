#pragma once

#include <memory>
#include <string>
#include <array>

class Window;
class Font;

struct Cursor {
    int row;
    int col;
};

class Display {
public:
    Display(int num_rows=25, int num_cols=80);
    ~Display();

    void set_cursor(int row, int col);
    void write_char(char c);
    void refresh();
    void scroll(int count);

private:
    void shift_rows();

    int num_rows;
    int num_cols;
    std::unique_ptr<Window> window;
    std::unique_ptr<Font> font;
    Cursor cursor;
    std::unique_ptr<char[]> characters;
};
