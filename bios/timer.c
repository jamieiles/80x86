#include "bios.h"
#include "io.h"
#include "timer.h"

static void timer_services(struct callregs *regs)
{
    switch (regs->ax.h) {
    case 0x0:
        regs->dx.x = readw(0x40, (void *)0x6c);
        regs->cx.x = readw(0x40, (void *)0x6e);
        regs->flags &= ~CF;
        break;
    case 0x1:
        writew(0x40, 0x6c, regs->dx.x);
        writew(0x40, 0x6e, regs->cx.x);
        regs->flags &= ~CF;
        break;
    default:
        regs->flags |= CF;
    }
}
VECTOR(0x1a, timer_services);

void init_timer(void)
{
    outb(0xfff5, 8);
    outb(0xfff4, 1);
    outw(0xffee, (1 << 15) | 55);
}
