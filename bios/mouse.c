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

#include "mouse.h"

#include "bios.h"
#include "io.h"

#define PS2_CTRL_CLEAR (1 << 7)
#define MOUSE_DATA 0xffe0
#define MOUSE_STATUS 0xffe1
#define MOUSE_IRQ 7

#define CMD_TIMEOUT_JIFFIES 20

extern unsigned short mouse_driver_addr[2];
extern int mouse_driver_enabled;
static int have_mouse;

static int mouse_cmd(unsigned char in, unsigned char *out)
{
    int enable_irqs = !irqs_enabled();
    unsigned long start = jiffies();
    int ret = -1;

    outb(MOUSE_DATA, in);

    *out = 0xfe;
    while (jiffies() < start + CMD_TIMEOUT_JIFFIES) {
        if (inb(MOUSE_STATUS) & (1 << 0)) {
            ret = 0;
            *out = inb(MOUSE_DATA);
            break;
        }

        // Allow the time to be updated
        if (enable_irqs) {
            sti();
            cli();
        }
    }

    return ret;
}

static int wait_self_test(void)
{
    while (!(inb(MOUSE_STATUS) & (1 << 0)))
        continue;
    unsigned char b = inb(MOUSE_DATA);

    if (b != 0xaa) {
        printk("self test failed (%02x)\n", b);
        return -1;
    }

    return 0;
}

int mouse_hw_init(void)
{
    outb(MOUSE_STATUS, PS2_CTRL_CLEAR);

    unsigned char resp;
    if (mouse_cmd(0xff, &resp) || resp != 0xfa) {
        printk("failed to reset mouse %02x\n", resp);
        return -1;
    }

    if (wait_self_test())
        return -1;

    if (mouse_cmd(0xf4, &resp) || resp != 0xfa) {
        printk("failed to enable scanning %02x\n", resp);
        return -1;
    }

    have_mouse = 1;

    return 0;
}

static void mouse_init(void)
{
    outb(MOUSE_STATUS, PS2_CTRL_CLEAR);
}

static void setup_mouse(struct callregs *regs)
{
    register unsigned short es asm("%es");

    mouse_driver_addr[1] = es;
    mouse_driver_addr[0] = regs->bx.x;

    if (have_mouse) {
        mouse_init();
        regs->flags &= ~CF;
    }
}

static void enable_mouse(struct callregs *regs)
{
    regs->flags &= ~CF;
    mouse_driver_enabled = regs->bx.h;
    if (mouse_driver_enabled)
        irq_enable(MOUSE_IRQ);
    else
        irq_disable(MOUSE_IRQ);
}

void mouse_services(struct callregs *regs)
{
    switch (regs->ax.l) {
    case 0x7:
        setup_mouse(regs);
        regs->ax.x = 0;
        break;
    case 0x0:
        enable_mouse(regs);
        regs->ax.x = 0;
        break;
    case 0x5: // Initialize interface
    case 0x3: // Set resolution
        regs->flags &= ~CF;
        regs->ax.x = 0;
        break;
    default: regs->flags |= CF;
    }
}
