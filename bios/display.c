#include "bios.h"
#include "serial.h"
#include "io.h"

static unsigned scan_start, scan_end;

static const unsigned frame_buffer_segment = 0xb000;
static const unsigned frame_buffer_offset = 0x8000;

union cursor {
    struct {
        unsigned char col;
        unsigned char row;
    } c;
    unsigned short v;
};

static void __attribute__((noinline)) read_cursor(union cursor *c)
{
    unsigned short pos;

    outb(0x3d4, 0xe);
    pos = ((unsigned short)inb(0x3d5)) << 8;
    outb(0x3d4, 0xf);
    pos |= inb(0x3d5);

    c->c.row = pos / 80;
    c->c.col = pos % 80;
}

static void __attribute__((noinline)) write_cursor(union cursor *c)
{
    unsigned short pos = (unsigned short)c->c.row * 80 + c->c.col;

    outb(0x3d4, 0xe);
    outb(0x3d5, pos >> 8);
    outb(0x3d4, 0xf);
    outb(0x3d5, pos & 0xff);

    bda_write(cursor_offsets[0], c->v);
}

static void __attribute__((noinline)) clear_row(unsigned char row,
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

static void __attribute__((noinline)) copy_row(unsigned char dst_row,
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

void __attribute__((noinline))
scroll_up(union cursor *cursor, struct callregs *regs)
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

void __attribute__((noinline))
scroll_down(union cursor *cursor, struct callregs *regs)
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

void __attribute__((noinline)) scroll_up_one(void)
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

static void __attribute__((noinline)) emit_char(char c)
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
    regs->cx.h = scan_start;
    regs->cx.l = scan_end;
    regs->dx.h = cursor.c.col;
    regs->dx.l = cursor.c.row;
    write_cursor(&cursor);
}

static void set_cursor_shape(struct callregs *regs)
{
    scan_start = regs->cx.h;
    scan_end = regs->cx.l;
}

static void get_video_mode(struct callregs *regs)
{
    regs->ax.h = 80;
    regs->ax.l = 0x03;
    regs->bx.h = 0;
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
    case 0x12:
        if (regs->bx.l == 0x10) {
            regs->bx.h = 0; // cursor.c.color
            regs->bx.l = 0; // 64K memory
            regs->cx.x = 0; // No features
        }
        break;
    default: regs->flags |= CF;
    }
}
VECTOR(0x10, video_services);

void display_init(void)
{
    unsigned r, c;

    for (r = 0; r < 25; ++r)
        for (c = 0; c < 80; ++c)
            writew(frame_buffer_segment, frame_buffer_offset + (r * 80 + c) * 2,
                   0x0720);

    bda_write(video_mode, 0x03);
    bda_write(num_screen_cols, 80);
    bda_write(last_screen_row, 25 - 1);
    bda_write(crt_controller_base, 0x3d4);

    union cursor cursor;
    cursor.c.col = cursor.c.row = 0;
    write_cursor(&cursor);
}
