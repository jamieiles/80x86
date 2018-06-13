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

#include "bda.h"
#include "bios.h"
#include "io.h"
#include "leds.h"
#include "scancodes.h"
#include "serial.h"
#include "utils.h"

#define PS2_DATA_PORT 0x60
#define PS2_CTRL_PORT 0x61
#define PS2_CTRL_CLEAR (1 << 7)
#define PS2_CTRL_RX_VALID (1 << 4)

static int keyboard_poll(void);

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

void noinline kbd_buffer_add(unsigned short key)
{
    if (kbd_buffer_full()) {
        led_set(LED_KBD_BUFFER_FULL);
        return;
    }

    led_clear(LED_KBD_BUFFER_FULL);
    led_set(LED_KBD_BUFFER_NON_EMPTY);

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

    if (head == bda_read(kbd_buffer_tail))
        led_clear(LED_KBD_BUFFER_NON_EMPTY);
}

unsigned kbd_buffer_peek(void)
{
    unsigned short head = bda_read(kbd_buffer_head);
    unsigned short tail = bda_read(kbd_buffer_tail);

    if (head == tail)
        while (keyboard_poll())
            continue;

    if (head == tail)
        return 0;

    unsigned circ_offset = head - offsetof(struct bios_data_area, kbd_buffer);

    return bda_read(kbd_buffer[circ_offset / sizeof(unsigned short)]);
}

#define SCANCODE_LSHIFT 0x2a
#define SCANCODE_LCTRL 0x1d
#define SCANCODE_LALT 0x38

#define KBD_FLAG_LSHIFT (1 << 1)
#define KBD_FLAG_LCTRL (1 << 2)
#define KBD_FLAG_LALT (1 << 3)

static int is_modifier(unsigned char b)
{
    b &= ~0x80;

    return b == SCANCODE_LALT || b == SCANCODE_LCTRL || b == SCANCODE_LSHIFT;
}

static void noinline modifier_key(unsigned char b, int keyup)
{
    unsigned char flags = bda_read(keyboard_flags[0]);
    unsigned char mask = 0;

    b &= ~0x80;

    if (b == SCANCODE_LALT)
        mask = KBD_FLAG_LALT;
    if (b == SCANCODE_LSHIFT)
        mask = KBD_FLAG_LSHIFT;
    if (b == SCANCODE_LCTRL)
        mask = KBD_FLAG_LCTRL;

    if (keyup)
        flags &= ~mask;
    else
        flags |= mask;

    bda_write(keyboard_flags[0], flags);
}

static int noinline get_kbd_flags(void)
{
    return bda_read(keyboard_flags[0]);
}

static void noinline keypress(const struct keydef *map, unsigned char b)
{
    const struct keydef *def = &map[b];
    unsigned short flags = get_kbd_flags();

    if ((flags & KBD_FLAG_LSHIFT) && def->shifted)
        kbd_buffer_add(def->shifted);
    else if ((flags & KBD_FLAG_LCTRL) && def->ctrl)
        kbd_buffer_add(def->ctrl);
    else if ((flags & KBD_FLAG_LALT) && def->alt)
        kbd_buffer_add(def->alt);
    else if (def->normal)
        kbd_buffer_add(def->normal);
}

static void keyboard_reset(void)
{
    outb(PS2_CTRL_PORT, inb(PS2_CTRL_PORT) | PS2_CTRL_CLEAR);
}

static int keyboard_poll(void)
{
    unsigned char b = inb(PS2_DATA_PORT);
    if (!b)
        return 0;

    outb(PS2_CTRL_PORT, inb(PS2_CTRL_PORT) | PS2_CTRL_CLEAR);

    int keyup = 0;
    if (b & 0x80)
        keyup = 1;

    if (is_modifier(b))
        modifier_key(b, keyup);
    else if (!keyup && b < ARRAY_SIZE(keycode_map))
        keypress(keycode_map, b);

    return 1;
}

static void keyboard_wait(struct callregs *regs)
{
    unsigned short c;

    regs->flags &= ~CF;
    do {
        c = kbd_buffer_peek();
        if (!c) {
#ifdef SERIAL_STDIO
            serial_poll();
#endif // SERIAL_STDIO
            while (keyboard_poll())
                continue;
        }
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
        keyboard_poll();
#ifdef SERIAL_STDIO
    if (!c)
        serial_poll();
#endif
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
    regs->ax.l = bda_read(keyboard_flags[0]);
}

static void keyboard_services(struct callregs *regs)
{
    regs->flags |= CF;

    switch (regs->ax.h) {
    case 0x0: // Fallthrough
    case 0x10: keyboard_wait(regs); break;
    case 0x1: // Fallthrough
    case 0x11: keyboard_status(regs); break;
    case 0x2: keyboard_shift_status(regs); break;
    default: break;
    }
}
VECTOR(0x16, keyboard_services);

void keyboard_init(void)
{
    bda_write(keyboard_flags[0], 0);
    bda_write(keyboard_flags[1], 0);
    bda_write(kbd_buffer_tail, offsetof(struct bios_data_area, kbd_buffer));
    bda_write(kbd_buffer_head, offsetof(struct bios_data_area, kbd_buffer));

    keyboard_reset();

    irq_enable(1);
}
