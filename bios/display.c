#include "bios.h"
#include "serial.h"
#include "io.h"

static unsigned row = 0;
static unsigned col = 0;
static unsigned scan_start, scan_end;

static const unsigned frame_buffer_segment = 0xb000;
static const unsigned frame_buffer_offset = 0x8000;

static void __attribute__((noinline)) scroll_up(unsigned count)
{
    unsigned start = (count * 80) * 2;
    unsigned shift_rows = count == 0 ? 0 : 25 - count;
    unsigned row, col;

    memcpy_seg(frame_buffer_segment, (void *)frame_buffer_offset,
               frame_buffer_segment, (void *)frame_buffer_offset + start,
               shift_rows * 80 * 2);

    for (row = shift_rows; row < 25; ++row)
        for (col = 0; col < 80; ++col)
            writew(frame_buffer_segment,
                   frame_buffer_offset + (row * 80 + col) * 2, 0);
}

static void do_backspace(void)
{
    int i;

    for (i = 0; i < 2 && col > 0; ++i) {
        --col;
        writeb(frame_buffer_segment,
               frame_buffer_offset + (row * 80 + col) * 2, ' ');
    }
}

static void __attribute__((noinline)) emit_char(char c)
{
    unsigned short v = 0x0f00 | c;

    writew(frame_buffer_segment, frame_buffer_offset + (row * 80 + col) * 2,
           v);
    ++col;

    if (c == '\b')
        do_backspace();

    if (c == '\r' || c == '\n') {
        unsigned m = 0;
        --col;
        for (m = col; m < 80; ++m)
            writew(frame_buffer_segment, frame_buffer_offset + (row * 80 + m) * 2, ' ');
    }

    if (col == 80 || c == '\n') {
        col = 0;
        ++row;
    }

    if (row == 25) {
        scroll_up(1);
        row = 24;
    }
}

static void read_char(struct callregs *regs)
{
    regs->ax.x = readw(frame_buffer_segment, frame_buffer_offset + (row * 80 + col) * 2);
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
    col = regs->dx.l;
    row = regs->dx.h;
}

static void get_cursor(struct callregs *regs)
{
    regs->cx.h = scan_start;
    regs->cx.l = scan_end;
    regs->dx.h = col;
    regs->dx.l = row;
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
    regs->flags &= ~CF;

    switch (regs->ax.h) {
    case 0xe:
        write_char(regs);
        break;
    case 0x2:
        set_cursor(regs);
        break;
    case 0x3:
        get_cursor(regs);
        break;
    case 0x1:
        set_cursor_shape(regs);
        break;
    case 0x6:
        scroll_up(regs->ax.l);
        break;
    case 0x7:
        scroll_up(regs->ax.l);
        break;
    case 0xf:
        get_video_mode(regs);
        break;
    case 0x8:
        read_char(regs);
        break;
    default:
        regs->flags |= CF;
    }
}
VECTOR(0x10, video_services);

void display_init(void)
{
    unsigned r, c;

    for (r = 0; r < 25; ++r)
        for (c = 0; c < 80; ++c)
            writew(frame_buffer_segment,
                   frame_buffer_offset + (r * 80 + c) * 2, 0x0720);

    bda_write(video_mode, 0x03);
    bda_write(num_screen_cols, 80);
    bda_write(crt_controller_base, 0x3d0);
}
