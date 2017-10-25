#include "bda.h"
#include "bios.h"
#include "display.h"
#include "sd.h"
#include "io.h"
#include "serial.h"
#include "keyboard.h"
#include "timer.h"
#include "utils.h"

#define INT11_DISKS_PRESENT (1 << 0)
#define INT11_CGA_ADAPTOR (2 << 4)

static void equipment_check(struct callregs *regs)
{
    regs->ax.x = INT11_CGA_ADAPTOR;
}
VECTOR(0x11, equipment_check);

static void serial_services(struct callregs *regs)
{
    regs->flags |= CF;
}
VECTOR(0x14, serial_services);

static void system_services(struct callregs *regs)
{
    regs->flags |= CF;
    regs->ax.h = 0x86;
}
VECTOR(0x15, system_services);

// Printer services
static void printer_services(struct callregs *regs)
{
    regs->flags |= CF;
}
VECTOR(0x17, printer_services);

static void basic_services(struct callregs __unused *regs)
{
    panic("No basic services\n");
}
VECTOR(0x18, basic_services);

static void boostrap_loader(struct callregs __unused *regs)
{
    asm volatile("jmp $0xffff, $0x0");
}
VECTOR(0x19, boostrap_loader);

static void conventional_memory(struct callregs *regs)
{
    regs->ax.x = 640;
}
VECTOR(0x12, conventional_memory);

static void break_handler(struct callregs __unused *regs)
{
    panic("No break handler\n");
}
VECTOR(0x1b, break_handler);

static void set_vector(int vector, void *handler)
{
    writew(0, vector * 4, (unsigned short)handler);
    writew(0, vector * 4 + 2, get_cs());
}

static unsigned char vpt[] = {80, 25, 8, 4000 & 0xff, 4000 >> 8};

static void install_vectors(void)
{
    struct vector {
        unsigned short num;
        void *handler;
    };

    extern struct vector vectors_start;
    extern struct vector vectors_end;

    struct vector *v;

    for (v = &vectors_start; v < &vectors_end; ++v)
        set_vector(v->num, v->handler);

    set_vector(0x1d, vpt);
}

static void bda_init(void)
{
    int i;

    for (i = 0; i < 256; ++i)
        writeb(0x40, i, 0);
}

#define PIC_COMMAND 0x20
#define PIC_DATA 0x21

#define PIC_ICW1_INIT (1 << 4)
#define PIC_ICW1_SINGLE (1 << 1)
#define PIC_ICW1_ICW4 (1 << 0)
#define PIC_ICW4_8086 (1 << 0)

static void irq_init(void)
{
    outb(PIC_COMMAND, PIC_ICW1_INIT | PIC_ICW1_SINGLE | PIC_ICW1_ICW4);
    outb(PIC_DATA, 0x08);
    outb(PIC_DATA, PIC_ICW4_8086);
}

void irq_enable(int irq_num)
{
    outb(PIC_DATA, inb(PIC_DATA) | (1 << irq_num));
}

void irq_ack(void)
{
    outb(0x20, 0x20);
}

void root(void)
{
    install_vectors();

    irq_init();
    bda_init();
    display_init();

    putstr("s80x86 BIOS, (C) Jamie Iles, " __DATE__ " " __TIME__ "\n");
    putstr("Platform: " __PLATFORM__ "\n");
    putstr("Build: " __BUILD__ "\n");
    putstr("\n");

    keyboard_init();
    init_timer();

    sd_init();
    sd_boot();
}
