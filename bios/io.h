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

#pragma once

static inline void outw(unsigned short port, unsigned short v)
{
    asm volatile("outw %1, %0" : : "d"(port), "a"(v));
}

static inline void outb(unsigned short port, unsigned char v)
{
    asm volatile(
        "outb %0, %w1"
        :
        : "Ral"(v), "Nd"(port));
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

    asm volatile(
        "inb %w1, %0\n"
        : "=Ral"(v)
        : "Nd"(port));

    return v;
}

#ifndef __FAR
static inline void writeb(unsigned short segment,
                          unsigned short address,
                          unsigned char val)
{
    asm volatile(
        "push %%ds\n"
        "mov %P[segment], %%ds\n"
        "mov %P[address], %%bx\n"
        "movb %P[val], (%%bx)\n"
        "pop %%ds"
        :
        : [segment] "r"(segment), [address] "m"(address), [val] "r"(val)
        : "%bx", "memory");
}

static inline void writew(unsigned short segment,
                          unsigned short address,
                          unsigned short val)
{
    asm volatile(
        "push %%ds\n"
        "mov %P[segment], %%ds\n"
        "mov %P[address], %%bx\n"
        "movw %P[val], (%%bx)\n"
        "pop %%ds"
        :
        : [segment] "r"(segment), [address] "m"(address), [val] "r"(val)
        : "%bx", "memory");
}
#else
static inline void writeb(unsigned short segment,
                          unsigned short address,
                          unsigned char val)
{
    unsigned char volatile __far *fp = (unsigned char volatile __far *)
        ((unsigned long)segment << 16 | address);
    *fp = val;
}

static inline void writew(unsigned short segment,
                          unsigned short address,
                          unsigned short val)
{
    unsigned short volatile __far *fp = (unsigned short volatile __far *)
        ((unsigned long)segment << 16 | address);
    *fp = val;
}
#endif

static inline void memcpy_seg(unsigned short dseg,
                              void *dst,
                              unsigned short sseg,
                              const void *src,
                              unsigned short len)
{
    asm volatile(
        "push %%ds\n"
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
        "pop %%ds\n"
        :
        : [dseg] "r"(dseg), [sseg] "r"(sseg), "S"(src), "D"(dst), [len] "r"(len)
        : "memory", "cc");
}

static inline void memset_seg(unsigned short dseg,
                              void *dst,
                              unsigned char v,
                              unsigned short len)
{
    asm volatile(
        "push %%ax\n"
        "push %%es\n"
        "push %%cx\n"
        "push %%di\n"
        "cld\n"
        "mov %P[dseg], %%es\n"
        "mov %[val], %%al\n"
        "mov %P[len], %%cx\n"
        "rep stosb\n"
        "pop %%di\n"
        "pop %%cx\n"
        "pop %%es\n"
        "pop %%ax\n"
        :
        : [dseg] "r"(dseg), [val] "r"(v), "D"(dst), [len] "r"(len)
        : "memory", "cc");
}

#ifndef __FAR
static inline unsigned char readb(unsigned short segment,
                                  unsigned short address)
{
    unsigned char v;

    asm volatile(
        "push %%ds\n"
        "mov %P[segment], %%ds\n"
        "mov %P[address], %%bx\n"
        "movb (%%bx), %P[val]\n"
        "pop %%ds"
        : [val] "=r"(v)
        : [segment] "r"(segment), [address] "m"(address)
        : "%bx", "memory");

    return v;
}

static inline unsigned short readw(unsigned short segment,
                                   unsigned short address)
{
    unsigned short v;

    asm volatile(
        "push %%ds\n"
        "mov %P[segment], %%ds\n"
        "mov %P[address], %%bx\n"
        "movw (%%bx), %P[val]\n"
        "pop %%ds"
        : [val] "=r"(v)
        : [segment] "r"(segment), [address] "m"(address)
        : "%bx", "memory");

    return v;
}
#else
static inline unsigned char readb(unsigned short segment,
                                  unsigned short address)
{
    unsigned char volatile __far *fp = (unsigned char volatile __far *)
        ((unsigned long)segment << 16 | address);
    return *fp;
}

static inline unsigned short readw(unsigned short segment,
                                   unsigned short address)
{
    unsigned short volatile __far *fp = (unsigned short volatile __far *)
        ((unsigned long)segment << 16 | address);
    return *fp;
}
#endif

static inline unsigned short get_cs(void)
{
    unsigned short cs;

    asm volatile("mov %%cs, %0" : "=r"(cs));

    return cs;
}
