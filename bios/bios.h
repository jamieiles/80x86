#pragma once

#include "bda.h"

#define __unused __attribute__((unused))

/* Pushed by the INT handlers in the order that a PUSHA would. */
union reg {
    struct {
        unsigned char l;
        unsigned char h;
    };
    unsigned short x;
};

struct callregs {
    union reg di;
    union reg si;
    union reg bp;
    union reg sp;
    union reg bx;
    union reg dx;
    union reg cx;
    union reg ax;
    unsigned short ds;
    unsigned short flags;
};

enum FlagBitPos {
    CF_OFFS = 0,
    PF_OFFS = 2,
    AF_OFFS = 4,
    ZF_OFFS = 6,
    SF_OFFS = 7,
    TF_OFFS = 8,
    IF_OFFS = 9,
    DF_OFFS = 10,
    OF_OFFS = 11
};

enum Flag {
    CF = (1 << CF_OFFS),
    PF = (1 << PF_OFFS),
    AF = (1 << AF_OFFS),
    ZF = (1 << ZF_OFFS),
    SF = (1 << SF_OFFS),
    TF = (1 << TF_OFFS),
    IF = (1 << IF_OFFS),
    DF = (1 << DF_OFFS),
    OF = (1 << OF_OFFS),
};

#define VECTOR(vnum, handler)                                     \
    static void __attribute__((used)) handler(struct callregs *); \
    asm(".pushsection .text, \"ax\"\n"                            \
        "1:\n"                                                    \
        "cli\n"                                                   \
        "push $" #handler                                         \
        "\n"                                                      \
        "jmp irq_entry\n"                                         \
        ".pushsection .rodata.vectors\n"                          \
        ".align 4\n"                                              \
        ".word " #vnum                                            \
        "\n"                                                      \
        ".word 1b\n"                                              \
        ".popsection")

#define bda_write(field, val)                                                \
    ({                                                                       \
        typeof(((struct bios_data_area *)0)->field) _p = (val);              \
        if (__builtin_types_compatible_p(typeof(_p), unsigned short))        \
            writew(0x40, offsetof(struct bios_data_area, field), _p);        \
        else if (__builtin_types_compatible_p(typeof(_p), unsigned char))    \
            writeb(0x40, offsetof(struct bios_data_area, field), _p);        \
        else                                                                 \
            memcpy_seg(0x40, (void *)offsetof(struct bios_data_area, field), \
                       get_cs(), &_p, sizeof(_p));                           \
    })

#define bda_read(field)                                                   \
    ({                                                                    \
        typeof(((struct bios_data_area *)0)->field) _p;                   \
        if (__builtin_types_compatible_p(typeof(_p), unsigned short))     \
            _p = readw(0x40, offsetof(struct bios_data_area, field));     \
        else if (__builtin_types_compatible_p(typeof(_p), unsigned char)) \
            _p = readb(0x40, offsetof(struct bios_data_area, field));     \
        else                                                              \
            memcpy_seg(get_cs(), &_p, 0x40,                               \
                       (void *)offsetof(struct bios_data_area, field),    \
                       sizeof(_p));                                       \
        _p;                                                               \
    })

#define offsetof __builtin_offsetof

void irq_enable(int irq_num);

#define ARRAY_SIZE(__a) (sizeof(__a) / sizeof(__a[0]))
