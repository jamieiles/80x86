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
#include "bda.h"
#include "io.h"
#include "serial.h"
#include "timer.h"

#define TIMER_PORT 0x40

static void timer_services(struct callregs *regs)
{
    switch (regs->ax.h) {
    case 0x0:
        regs->dx.x = bda_read(timer_counter_low);
        regs->cx.x = bda_read(timer_counter_high);
        regs->ax.l = 0;
        regs->flags &= ~CF;
        break;
    case 0x1:
        bda_write(timer_counter_low, regs->dx.x);
        bda_write(timer_counter_high, regs->cx.x);
        regs->flags &= ~CF;
        break;
    case 0x2:
        regs->cx.h = BIOS_BUILD_HOUR;
        regs->cx.l = BIOS_BUILD_MINUTE;
        regs->dx.h = BIOS_BUILD_SECOND;
        regs->flags &= ~CF;
        break;
    case 0x4:
        regs->cx.x = BIOS_BUILD_YEAR;
        regs->dx.h = BIOS_BUILD_MONTH;
        regs->dx.l = BIOS_BUILD_DAY;
        regs->flags &= ~CF;
        break;
    default: regs->flags |= CF;
    }
}
VECTOR(0x1a, timer_services);

void init_timer(void)
{
    irq_enable(0);
    outb(TIMER_PORT + 0x3, 0x34);
    outb(TIMER_PORT + 0x0, 0x00);
    outb(TIMER_PORT + 0x0, 0x00);
}
