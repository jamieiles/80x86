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

#include "bios.h"
#include "serial.h"
#include "io.h"

static int _in_video_mode;

int in_video_mode(void)
{
    return _in_video_mode;
}

static const unsigned frame_buffer_segment = 0xb000;
static const unsigned frame_buffer_offset = 0x8000;

union cursor {
    struct {
        unsigned char col;
        unsigned char row;
    } c;
    unsigned short v;
};

static void noinline crtc_reg_write(unsigned char reg, unsigned char val)
{
    outb(0x3d4, reg);
    outb(0x3d5, val);
}

static unsigned char noinline crtc_reg_read(unsigned char reg)
{
    outb(0x3d4, reg);

    return inb(0x3d5);
}

static void noinline read_cursor(union cursor *c)
{
    unsigned short pos;

    pos = (unsigned short)crtc_reg_read(0xe) << 8;
    pos |= crtc_reg_read(0xf);

    c->c.row = pos / 80;
    c->c.col = pos % 80;
}

static void noinline write_cursor(union cursor *c)
{
    unsigned short pos = (unsigned short)c->c.row * 80 + c->c.col;

    crtc_reg_write(0xe, pos >> 8);
    crtc_reg_write(0xf, pos);

    bda_write(cursor_offsets[0], c->v);
}

static void noinline clear_row(unsigned char row,
                               unsigned char col_left,
                               unsigned char col_right,
                               unsigned char attr)
{
    unsigned char col;
    const signed short blank = ((signed short)attr << 8) | 0x20;

    for (col = col_left; col <= col_right; ++col)
        writew(frame_buffer_segment, frame_buffer_offset + (row * 80 + col) * 2,
               blank);
}

static void noinline copy_row(unsigned char dst_row,
                              unsigned char src_row,
                              unsigned char col_left,
                              unsigned char col_right,
                              unsigned char attr)
{
    unsigned char col;

    for (col = col_left; col <= col_right; ++col) {
        unsigned short b =
            readw(frame_buffer_segment,
                  frame_buffer_offset + (src_row * 80 + col) * 2);
        writew(frame_buffer_segment,
               frame_buffer_offset + (dst_row * 80 + col) * 2, b);
    }
}

void __scroll_up(signed char row_top,
                 signed char row_bottom,
                 signed char col_left,
                 signed char col_right,
                 signed char blank_attr,
                 signed char scroll_count)
{
    signed char row;

    for (row = row_top; row <= row_bottom; ++row) {
        if (row + scroll_count > row_bottom)
            clear_row(row, col_left, col_right, blank_attr);
        else
            copy_row(row, row + scroll_count, col_left, col_right, blank_attr);
    }
}

void noinline scroll_up(union cursor *cursor, struct callregs *regs)
{
    signed char row_top = regs->cx.h;
    signed char row_bottom = regs->dx.h;
    signed char col_left = regs->cx.l;
    signed char col_right = regs->dx.l;
    signed char blank_attr = regs->bx.h;
    signed char scroll_count = regs->ax.l == 0 ? 25 : regs->ax.l;

    __scroll_up(row_top, row_bottom, col_left, col_right, blank_attr,
                scroll_count);
}

static void __scroll_down(signed char row_top,
                          signed char row_bottom,
                          signed char col_left,
                          signed char col_right,
                          signed char blank_attr,
                          signed char scroll_count)
{
    signed char row;

    for (row = row_bottom; row >= row_top; --row) {
        if (row - scroll_count < 0)
            clear_row(row, col_left, col_right, blank_attr);
        else
            copy_row(row, row - scroll_count, col_left, col_right, blank_attr);
    }
}

void noinline scroll_down(union cursor *cursor, struct callregs *regs)
{
    signed char row_top = regs->cx.h;
    signed char row_bottom = regs->dx.h;
    signed char col_left = regs->cx.l;
    signed char col_right = regs->dx.l;
    signed char blank_attr = regs->bx.h;
    signed char scroll_count = regs->ax.l == 0 ? 25 : regs->ax.l;

    __scroll_down(row_top, row_bottom, col_left, col_right, blank_attr,
                  scroll_count);
}

void noinline scroll_up_one(void)
{
    __scroll_up(0, 24, 0, 79, 0x07, 1);
}

static void do_backspace(union cursor *cursor)
{
    int i;

    for (i = 0; i < 2 && cursor->c.col > 0; ++i) {
        --cursor->c.col;
        writeb(frame_buffer_segment,
               frame_buffer_offset + (cursor->c.row * 80 + cursor->c.col) * 2,
               ' ');
    }
}

static void noinline emit_char(char c)
{
    union cursor cursor;

    read_cursor(&cursor);

    writeb(frame_buffer_segment,
           frame_buffer_offset + (cursor.c.row * 80 + cursor.c.col) * 2, c);
    ++cursor.c.col;

    if (c == '\b')
        do_backspace(&cursor);

    if (c == '\r' || c == '\n') {
        unsigned m = 0;
        --cursor.c.col;
        for (m = cursor.c.col; m < 80; ++m)
            writew(frame_buffer_segment,
                   frame_buffer_offset + (cursor.c.row * 80 + m) * 2, 0x0720);
    }

    if (cursor.c.col == 80 || c == '\n') {
        cursor.c.col = 0;
        ++cursor.c.row;
    }

    if (cursor.c.row >= 25) {
        scroll_up_one();
        cursor.c.row = 24;
    }
    write_cursor(&cursor);
}

static void read_char(struct callregs *regs)
{
    union cursor cursor;

    read_cursor(&cursor);
    regs->ax.x =
        readw(frame_buffer_segment,
              frame_buffer_offset + (cursor.c.row * 80 + cursor.c.col) * 2);
    write_cursor(&cursor);
}

static void write_char_and_attribute(struct callregs *regs)
{
    union cursor cursor;

    read_cursor(&cursor);
    unsigned short v = ((unsigned short)regs->bx.l << 8) | regs->ax.l;
    writew(frame_buffer_segment,
           frame_buffer_offset + (cursor.c.row * 80 + cursor.c.col) * 2, v);
}

void video_putchar(char c)
{
    if (c == '\r')
        return;

    emit_char(c);
}

static void write_char(struct callregs *regs)
{
    putchar(regs->ax.l);
}

static void set_cursor(struct callregs *regs)
{
    union cursor cursor;

    read_cursor(&cursor);
    cursor.c.col = regs->dx.l;
    cursor.c.row = regs->dx.h;
    write_cursor(&cursor);
}

static void get_cursor(struct callregs *regs)
{
    union cursor cursor;

    read_cursor(&cursor);
    regs->cx.h = bda_read(cursor_start);
    regs->cx.l = bda_read(cursor_end);
    regs->dx.h = cursor.c.row;
    regs->dx.l = cursor.c.col;
    write_cursor(&cursor);
}

static void noinline set_cursor_scan_start(unsigned char scan_start)
{
    // Cursor disabled requested
    if (scan_start & 0x20)
        scan_start = 0x10 | (scan_start & 0xf);
    crtc_reg_write(0xa, scan_start);
}

static void noinline set_cursor_scan_end(unsigned char scan_end)
{
    crtc_reg_write(0xb, scan_end);
}

static void noinline set_cursor_bda(unsigned char scan_start,
                                    unsigned char scan_end)
{
    bda_write(cursor_start, scan_start);
    bda_write(cursor_end, scan_end);
}

static void noinline __set_cursor_shape(unsigned char scan_start,
                                        unsigned char scan_end)
{
    set_cursor_scan_end(scan_end);
    set_cursor_scan_start(scan_start);

    set_cursor_bda(scan_start, scan_end);
}

static void noinline set_cursor_shape(struct callregs *regs)
{
    unsigned char scan_start = regs->cx.h;
    unsigned char scan_end = regs->cx.l;

    __set_cursor_shape(scan_start, scan_end);
}

static void get_video_mode(struct callregs *regs)
{
    if (inb(0x3d8) & (1 << 0))
        regs->ax.l = 3;
    else
        regs->ax.l = 5;
    regs->ax.h = 80;
    regs->bx.h = 0;
}

static int __set_video_mode(int mode)
{
    switch (mode) {
    case 0x2:
    case 0x3: // 80x25 chars B&W
        outb(0x3d8, (1 << 0) | (1 << 3));
        _in_video_mode = 0;
        __scroll_up(0, 24, 0, 79, 0x70, 0);
        break;
    case 0x4: // 320x200 4 color graphics
    case 0x5: // 320x200 4 color graphics
        outb(0x3d8, (1 << 1) | (1 << 3));
        memset_seg(frame_buffer_segment, (void *)frame_buffer_offset, 0,
                   (320LU * 200LU) / 4LU);
        _in_video_mode = 1;
        break;
    default: return -1;
    }

    return 0;
}

static void set_video_mode(struct callregs *regs)
{
    if (__set_video_mode(regs->ax.l))
        regs->flags |= CF;
}

static void video_services(struct callregs *regs)
{
    union cursor cursor;
    regs->flags &= ~CF;

    switch (regs->ax.h) {
    case 0xe: write_char(regs); break;
    case 0x2: set_cursor(regs); break;
    case 0x3: get_cursor(regs); break;
    case 0x1: set_cursor_shape(regs); break;
    case 0x6:
        read_cursor(&cursor);
        scroll_up(&cursor, regs);
        write_cursor(&cursor);
        break;
    case 0x7:
        read_cursor(&cursor);
        scroll_down(&cursor, regs);
        write_cursor(&cursor);
        break;
    case 0xf: get_video_mode(regs); break;
    case 0x9: write_char_and_attribute(regs); break;
    case 0x8: read_char(regs); break;
    case 0x0: set_video_mode(regs); break;
    case 0x10:
        break; // Set palette
    default: regs->flags |= CF;
    }
}
VECTOR(0x10, video_services);

void display_init(void)
{
    unsigned r, c;

    __set_video_mode(3);

    for (r = 0; r < 25; ++r)
        for (c = 0; c < 80; ++c)
            writew(frame_buffer_segment, frame_buffer_offset + (r * 80 + c) * 2,
                   0x0720);

    bda_write(video_mode, 0x03);
    bda_write(num_screen_cols, 80);
    bda_write(last_screen_row, 25 - 1);
    bda_write(crt_controller_base, 0x3d4);

    __set_cursor_shape(7, 7);

    union cursor cursor;
    cursor.c.col = cursor.c.row = 0;
    write_cursor(&cursor);
}
