#include "bios.h"
#include "io.h"
#include "serial.h"

static char last_key;

static void keyboard_wait(struct callregs *regs)
{
    regs->flags &= ~CF;

    char c = read_csbyte(&last_key);
    if (!c)
        c = getchar();

    write_csbyte(&last_key, 0);
    regs->ax.l = c;
    regs->ax.h = c;
}

static void keyboard_status(struct callregs *regs)
{
    regs->flags &= ~CF;

    char c = read_csbyte(&last_key);
    if (!c && getchar_ready())
        c = getchar();

    write_csbyte(&last_key, c);
    if (c)
        regs->flags &= ~ZF;
    else
        regs->flags |= ZF;

    regs->ax.l = c;
    regs->ax.h = c;
}

static void keyboard_shift_status(struct callregs *regs)
{
    regs->flags &= ~CF;
    regs->ax.h = 0;
}

void int16_function(struct callregs *regs)
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

void keyboard_init(void)
{
    write_csbyte(&last_key, 0);
}
