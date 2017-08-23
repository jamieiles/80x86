#include "bios.h"
#include "io.h"
#include "timer.h"

static unsigned long time_count;

static void timer_services(struct callregs *regs)
{
    switch (regs->ax.h) {
    case 0x0:
        regs->cx.x = time_count >> 16;
        regs->dx.x = time_count;
        regs->flags &= ~CF;
        break;
    case 0x1:
        time_count = (regs->dx.x |
                      ((unsigned long)regs->cx.x << 16));
        regs->flags &= ~CF;
        break;
    default:
        regs->flags |= CF;
    }
}
VECTOR(0x1a, timer_services);

static void timer_irq(struct callregs *regs)
{
    ++time_count;

    memcpy_seg(0x40, (void *)0x006c, get_cs(), &time_count,
               sizeof(time_count));

    asm volatile("int $0x1c");

    (void)inw(0xffee);
}
VECTOR(0x08, timer_irq);

void init_timer(void)
{
    outb(0xfff5, 8);
    outb(0xfff4, 1);
    outw(0xffee, (1 << 15) | 55);

    time_count = 0;
}
