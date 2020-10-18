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

#define CMD_TIMEOUT 32768U

#define CMD_REFRESH 0xf3
#define CMD_DISABLE 0xf5
#define CMD_ENABLE 0xf4

extern unsigned short mouse_driver_addr[2];
extern int mouse_driver_enabled;
static int have_mouse;

static int mouse_read(unsigned char *out)
{
    if (!(inb(MOUSE_STATUS) & (1 << 4)))
        return -1;

    *out = inb(MOUSE_DATA);

    return 0;
}

static int mouse_cmd(unsigned char in, unsigned char *out)
{
    int ret = -1;
    unsigned m = 0;

    outb(MOUSE_DATA, in);

    *out = 0xfe;
    do {
        if (!mouse_read(out)) {
            ret = 0;
            break;
        }
    } while (m++ < CMD_TIMEOUT);

    return ret;
}

static int wait_self_test(void)
{
    unsigned char b;

    while (mouse_read(&b))
        continue;

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

    if (mouse_cmd(CMD_ENABLE, &resp) || resp != 0xfa) {
        printk("failed to enable scanning %02x\n", resp);
        return -1;
    }

    mouse_cmd(CMD_REFRESH, &resp);
    mouse_cmd(40, &resp);

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

extern unsigned char mouse_nbytes;

void mouse_suspend(void)
{
    if (!have_mouse)
        return;

    unsigned char o;
    mouse_cmd(CMD_DISABLE, &o);
}

void mouse_resume(void)
{
    if (!have_mouse)
        return;

    mouse_init();
    mouse_nbytes = 0;

    unsigned char o;
    mouse_cmd(CMD_ENABLE, &o);

    mouse_init();
}
