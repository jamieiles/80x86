#include "bda.h"
#include "bios.h"
#include "io.h"
#include "serial.h"
#include "utils.h"

static void keyboard_wait(struct callregs *regs)
{
    unsigned short c ;

    regs->flags &= ~CF;
    do {
        if (!bda_read(kbd_buffer[0]))
            serial_poll();
        c = bda_read(kbd_buffer[0]);
        bda_write(kbd_buffer[0], 0);
        bda_write(kbd_buffer_tail, offsetof(struct bios_data_area, kbd_buffer));
    } while (c == 0);

    regs->ax.x = c;
}

static void keyboard_status(struct callregs *regs)
{
    unsigned short c;

    regs->flags &= ~CF;

    if (!bda_read(kbd_buffer[0]))
        serial_poll();
    c = bda_read(kbd_buffer[0]);
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
    bda_write(kbd_buffer_tail, 0);
    bda_write(kbd_buffer_head, 0);
}
