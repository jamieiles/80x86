#include "bios.h"
#include "sd.h"
#include "io.h"
#include "serial.h"
#include "keyboard.h"

void *memset(void *s, int c, unsigned short n)
{
    unsigned char *p = s;
    unsigned short m;

    for (m = 0; m < n; ++m)
        p[m] = c;

    return s;
}

void panic(const char *p)
{
    putstr(p);
    putstr("\n\r\n\rHalted.\n\r");

    for (;;)
        continue;
}

void int10_function(struct callregs *regs)
{
    if (regs->ax.h == 0xe) {
        putchar(regs->ax.l);
        regs->flags &= ~CF;
    } else {
        regs->flags |= CF;
    }
}

#define INT11_DISKS_PRESENT (1 << 0)
#define INT11_MDA_ADAPTOR (3 << 4)

void int11_function(struct callregs *regs)
{
    regs->ax.x = INT11_DISKS_PRESENT | INT11_MDA_ADAPTOR;
}

// Serial services
void int14_function(struct callregs *regs)
{
    regs->flags |= CF;
}

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

void int15_function(struct callregs *regs)
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

// Printer services
void int17_function(struct callregs *regs)
{
    regs->flags |= CF;
}

void int18_function(struct callregs *regs)
{
    (void)regs;
    panic("No basic services\n\r");
}

void int19_function(struct callregs *regs)
{
    (void)regs;
    asm volatile("jmp $0xffff, $0x0");
}

void int12_function(struct callregs *regs)
{
    regs->ax.x = 640;
}

static unsigned long time_count;

void int1a_function(struct callregs *regs)
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

void int1b_function(struct callregs *regs)
{
    (void)regs;
    panic("No break handler\n\r");
}

void int1c_function(struct callregs *regs)
{
    (void)regs;
    panic("No tick handler\n\r");
}

void set_vector(int vector, void (*handler)(void))
{
    writew(0, vector * 4, (unsigned short)handler);
    writew(0, vector * 4 + 2, get_cs());
}

void timer_function(struct callregs *regs)
{
    ++time_count;

    memcpy_seg(0x40, (void *)0x006c, get_cs(), &time_count,
               sizeof(time_count));

    (void)inw(0xffee);
}

static void init_timer(void)
{
    outb(0xfff5, 8);
    outb(0xfff4, 1);
    outw(0xffee, (1 << 15) | 55);

    time_count = 0;
}

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
}

void root(void)
{
    putstr("s80x86 BIOS, (C) Jamie Iles 2017, " __DATE__ " " __TIME__ "\r\n");
    putstr("Platform: " __PLATFORM__ "\r\n");
    putstr("Build: " __BUILD__ "\r\n");
    putstr("\r\n");

    keyboard_init();
    install_vectors();
    putstr("Init timers\n\r");
    init_timer();
    putstr("Initalization done\n\r");

    sd_init();
    sd_boot();
}
