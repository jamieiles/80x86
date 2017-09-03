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
    regs->ax.x = INT11_DISKS_PRESENT | INT11_CGA_ADAPTOR;
}
VECTOR(0x11, equipment_check);

static void serial_services(struct callregs *regs)
{
    regs->flags |= CF;
}
VECTOR(0x14, serial_services);

static const unsigned char bios_params_rec[] = {
    8, 0, 0xff, 0, 0, 0, 0, 0
};

static void system_configuration_parameters(struct callregs *regs)
{
    regs->flags &= ~CF;
    regs->ax.h = 0x80;
    set_es(get_cs());
    regs->bx.x = (unsigned short)bios_params_rec;
}

static void wait_event(struct callregs *regs)
{
    regs->flags &= ~CF;
}

static void system_extended_memory_size(struct callregs *regs)
{
    regs->flags &= ~CF;
    regs->ax.x = 0;
}

static void a20_gate(struct callregs *regs)
{
    regs->flags |= CF;
}

static void system_services(struct callregs *regs)
{
    regs->flags |= CF;

    switch (regs->ax.h) {
    case 0xc0:
        system_configuration_parameters(regs);
        break;
    case 0x41:
        wait_event(regs);
        break;
    case 0x88:
        system_extended_memory_size(regs);
        break;
    case 0x24:
        a20_gate(regs);
        break;
    default:
        break;
    }
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

static unsigned char vpt[] = {
        80, 25, 8, 4000 & 0xff, 4000 >> 8
};

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

void root(void)
{
    bda_init();
    display_init();

    putstr("s80x86 BIOS, (C) Jamie Iles, " __DATE__ " " __TIME__ "\n");
    putstr("Platform: " __PLATFORM__ "\n");
    putstr("Build: " __BUILD__ "\n");
    putstr("\n");

    keyboard_init();
    install_vectors();
    putstr("Init timers\n");
    init_timer();
    putstr("Initalization done\n");

    sd_init();
    sd_boot();
}
