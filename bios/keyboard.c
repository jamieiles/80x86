#include "bda.h"
#include "bios.h"
#include "io.h"
#include "scancodes.h"
#include "serial.h"
#include "utils.h"

#define PS2_DATA_PORT 0x60
#define PS2_CTRL_PORT 0x61
#define PS2_CTRL_ACK (1 << 0)
#define PS2_CTRL_RX_VALID (1 << 0)

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

void __attribute__((noinline)) kbd_buffer_add(unsigned short key)
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

#define SCANCODE_LSHIFT 0x12
#define SCANCODE_LCTRL 0x14
#define SCANCODE_LALT 0x11

#define KBD_FLAG_LSHIFT (1 << 1)
#define KBD_FLAG_LCTRL (1 << 2)
#define KBD_FLAG_LALT (1 << 3)

static int is_modifier(unsigned char b)
{
    return b == SCANCODE_LALT || b == SCANCODE_LCTRL || b == SCANCODE_LSHIFT;
}

void modifier_key(unsigned char b)
{
    unsigned char flags = bda_read(keyboard_flags[0]);

    if (b == SCANCODE_LALT)
        flags ^= KBD_FLAG_LALT;
    if (b == SCANCODE_LSHIFT)
        flags ^= KBD_FLAG_LSHIFT;
    if (b == SCANCODE_LCTRL)
        flags ^= KBD_FLAG_LCTRL;

    bda_write(keyboard_flags[0], flags);
}

static int __attribute__((noinline)) get_kbd_flags(void)
{
    return bda_read(keyboard_flags[0]);
}

static void __attribute__((noinline))
keypress(const struct keydef *map, unsigned char b)
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

static unsigned char get_scancode(void)
{
    unsigned char b;

    do {
        b = inb(PS2_DATA_PORT);
        if (b)
            outb(PS2_CTRL_PORT, PS2_CTRL_ACK);
    } while (!b);

    return b;
}

static int is_extended(unsigned char b)
{
    return b == 0xe0;
}

static void extended_key(void)
{
    keypress(extended_keycode_map, get_scancode());
}

static void keyboard_reset(void)
{
    // Reset
    outb(PS2_DATA_PORT, 0xff);
    // Flush FIFO
    int i;
    for (i = 0; i < 64; ++i) {
        if (!(inb(PS2_CTRL_PORT) & PS2_CTRL_RX_VALID))
            break;
        outb(PS2_CTRL_PORT, PS2_CTRL_ACK);
    }
    if (i == 64)
        putstr("Warning: failed to empty Keyboard FIFO.\n");
}

static void keyboard_poll(void)
{
    unsigned char b = inb(PS2_DATA_PORT);
    if (!b)
        return;

    if (b == 0xaa) {
        keyboard_reset();
        return;
    }

    outb(PS2_CTRL_PORT, PS2_CTRL_ACK);

    int keyup = 0;
    if (b == 0xf0) {
        keyup = 1;
        b = get_scancode();
    }

    if (is_modifier(b))
        modifier_key(b);
    else if (is_extended(b))
        extended_key();
    else if (!keyup && b < ARRAY_SIZE(keycode_map))
        keypress(keycode_map, b);
}

static void kbd_irq(struct callregs *regs)
{
    keyboard_poll();
}
VECTOR(0x9, kbd_irq);

static void keyboard_wait(struct callregs *regs)
{
    unsigned short c;

    regs->flags &= ~CF;
    do {
        c = kbd_buffer_peek();
        if (!c) {
            serial_poll();
            keyboard_poll();
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

    // Enable IRQ
    outb(0xfff4, inb(0xfff4) | 2);
}
