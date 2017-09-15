#pragma once

struct Cursor {
    Cursor(unsigned row_top,
           unsigned row_bottom,
           unsigned col_left,
           unsigned col_right,
           bool enabled)
        : row_top(row_top),
          row_bottom(row_bottom),
          col_left(col_left),
          col_right(col_right),
          enabled(enabled)
    {
    }
    bool is_active_at(unsigned int row, unsigned int col) const
    {
        return enabled && row >= row_top && row <= row_bottom &&
               col >= col_left && col <= col_right;
    }

private:
    unsigned row_top, row_bottom;
    unsigned col_left, col_right;
    bool enabled;
};
