#pragma once

static inline void outw(unsigned short port, unsigned short v)
{
    asm volatile("outw %1, %0" : : "d"(port), "a"(v));
}

static inline void outb(unsigned short port, unsigned char v)
{
    asm volatile("mov %0, %%al\n"
                 "outb %%al, %w1" : : "a"(v), "Nd"(port) : "%al");
}

static inline unsigned short inw(unsigned short port)
{
    unsigned short v;

    asm volatile("inw %1, %0" : "=a"(v) : "d"(port));

    return v;
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char v;

    asm volatile("inb %w1, %%al\n"
                 "mov %%al, %0" : "=a"(v) : "Nd"(port) : "%al");

    return v;
}

static inline void writeb(unsigned short segment, unsigned short address,
			  unsigned char val)
{
    asm volatile("push %%ds\n"
                 "mov %P[segment], %%ds\n"
                 "mov %P[address], %%bx\n"
                 "movb %P[val], (%%bx)\n"
                 "pop %%ds" : : [segment]"r"(segment),
                                [address]"m"(address),
                                [val]"r"(val) : "%bx", "memory");
}

static inline void writew(unsigned short segment, unsigned short address,
			  unsigned short val)
{
    asm volatile("push %%ds\n"
                 "mov %P[segment], %%ds\n"
                 "mov %P[address], %%bx\n"
                 "movw %P[val], (%%bx)\n"
                 "pop %%ds" : : [segment]"r"(segment),
                                [address]"m"(address),
                                [val]"r"(val) : "%bx", "memory");
}

static inline void memcpy_seg(unsigned short dseg, void *dst,
			      unsigned short sseg, const void *src,
			      unsigned short len)
{
    asm volatile("push %%ds\n"
                 "push %%es\n"
                 "push %%cx\n"
                 "push %%si\n"
                 "push %%di\n"
                 "cld\n"
                 "mov %P[dseg], %%es\n"
                 "mov %P[sseg], %%ds\n"
                 "mov %P[len], %%cx\n"
                 "rep movsb\n"
                 "pop %%di\n"
                 "pop %%si\n"
                 "pop %%cx\n"
                 "pop %%es\n"
                 "pop %%ds\n" : : [dseg]"r"(dseg), [sseg]"r"(sseg),
                    "S"(src), "D"(dst), [len]"r"(len) : "memory", "cc");
}

static inline unsigned char readb(unsigned short segment, unsigned short address)
{
    unsigned char v;

    asm volatile("push %%ds\n"
                 "mov %P[segment], %%ds\n"
                 "mov %P[address], %%bx\n"
                 "movb (%%bx), %P[val]\n"
                 "pop %%ds" : [val]"=r"(v) : [segment]"r"(segment),
                                [address]"m"(address) : "%bx", "memory");

    return v;
}

static inline unsigned short readw(unsigned short segment, unsigned short address)
{
    unsigned short v;

    asm volatile("push %%ds\n"
                 "mov %P[segment], %%ds\n"
                 "mov %P[address], %%bx\n"
                 "movw (%%bx), %P[val]\n"
                 "pop %%ds" : [val]"=r"(v) : [segment]"r"(segment),
                                [address]"m"(address) : "%bx", "memory");

    return v;
}

static inline unsigned short get_cs(void)
{
    unsigned short cs;

    asm volatile("mov %%cs, %0" : "=r"(cs));

    return cs;
}

static inline unsigned short get_es(void)
{
    unsigned short es;

    asm volatile("mov %%es, %0" : "=r"(es));

    return es;
}

static inline void set_es(unsigned short es)
{
    asm volatile("mov %0, %%es" :: "r"(es));
}
