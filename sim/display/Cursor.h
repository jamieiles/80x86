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
