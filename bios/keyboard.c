#include "bda.h"
#include "bios.h"
#include "io.h"
#include "serial.h"
#include "utils.h"

static int kbd_buffer_full(void)
{
    unsigned short tail = bda_read(kbd_buffer_tail);
    unsigned short head = bda_read(kbd_buffer_head);

    // clang-format off
    return (tail == head - 2) ||
           (head == offsetof(struct bios_data_area, kbd_buffer) &&
            tail == (offsetof(struct bios_data_area, drive_recalibration_status) - 2));
    // clang-format on
}

void kbd_buffer_add(unsigned short key)
{
    if (kbd_buffer_full())
        return;

    unsigned short tail = bda_read(kbd_buffer_tail);
    unsigned circ_offset = tail - offsetof(struct bios_data_area, kbd_buffer);
    bda_write(kbd_buffer[circ_offset / sizeof(unsigned short)], key);

    tail += sizeof(unsigned short);
    if (tail >= offsetof(struct bios_data_area, drive_recalibration_status))
        tail = offsetof(struct bios_data_area, kbd_buffer);
    bda_write(kbd_buffer_tail, tail);
}

static void kbd_buffer_pop(void)
{
    unsigned short head = bda_read(kbd_buffer_head);

    head += sizeof(unsigned short);
    if (head >= offsetof(struct bios_data_area, drive_recalibration_status))
        head = offsetof(struct bios_data_area, kbd_buffer);
    bda_write(kbd_buffer_head, head);
}

unsigned kbd_buffer_peek(void)
{
    unsigned short head = bda_read(kbd_buffer_head);
    unsigned short tail = bda_read(kbd_buffer_tail);

    if (head == tail)
        return 0;

    unsigned circ_offset = head - offsetof(struct bios_data_area, kbd_buffer);

    return bda_read(kbd_buffer[circ_offset / sizeof(unsigned short)]);
}

static void keyboard_wait(struct callregs *regs)
{
    unsigned short c;

    regs->flags &= ~CF;
    do {
        c = kbd_buffer_peek();
        if (!c)
            serial_poll();
        c = kbd_buffer_peek();
    } while (c == 0);

    kbd_buffer_pop();

    regs->ax.x = c;
}

static void keyboard_status(struct callregs *regs)
{
    unsigned short c;

    regs->flags &= ~CF;

    c = kbd_buffer_peek();
    if (!c)
        serial_poll();
    c = kbd_buffer_peek();
    if (c)
        regs->flags &= ~ZF;
    else
        regs->flags |= ZF;

    regs->ax.x = c;
}

static void keyboard_shift_status(struct callregs *regs)
{
    regs->flags &= ~CF;
    regs->ax.h = 0;
    regs->ax.l = 0;
}

static void keyboard_services(struct callregs *regs)
{
    regs->flags |= CF;

    switch (regs->ax.h) {
        case 0x0: // Fallthrough
        case 0x10:
            keyboard_wait(regs);
            break;
        case 0x1: // Fallthrough
        case 0x11:
            keyboard_status(regs);
            break;
        case 0x2:
            keyboard_shift_status(regs);
            break;
        default:
            break;
    }
}
VECTOR(0x16, keyboard_services);

void keyboard_init(void)
{
    bda_write(keyboard_flags[0], 0);
    bda_write(keyboard_flags[1], 0);
    bda_write(kbd_buffer_tail, offsetof(struct bios_data_area, kbd_buffer));
    bda_write(kbd_buffer_head, offsetof(struct bios_data_area, kbd_buffer));
}
