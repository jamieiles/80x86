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
#include "display.h"
#include "bda.h"
#include "io.h"

extern void video_putchar(char c);

#define UART_DATA_PORT 0xfffa
#define UART_STATUS_PORT 0xfffb
#define UART_TX_BUSY 0x02
#define UART_RX_READY 0x01

void putchar(unsigned char c)
{
#ifdef SERIAL_STDIO
    outb(UART_DATA_PORT, c);

    while (inb(UART_STATUS_PORT) & UART_TX_BUSY)
        continue;
#endif // SERIAL_STDIO

    if (!in_video_mode())
        video_putchar(c);

    if (c == '\n')
        putchar('\r');
}

void serial_putchar(unsigned char c)
{
    outb(UART_DATA_PORT, c);

    while (inb(UART_STATUS_PORT) & UART_TX_BUSY)
        continue;
    if (c == '\n')
        serial_putchar('\r');
}

extern void kbd_buffer_add(unsigned short key);

int serial_poll(void)
{
    int ready = inb(UART_STATUS_PORT) & UART_RX_READY;

    if (ready)
        kbd_buffer_add(inb(UART_DATA_PORT));

    return ready;
}

void putbyte(unsigned char b)
{
    static const char hexarr[] = "0123456789abcdef";

    putchar(hexarr[(b & 0xf0) >> 4]);
    putchar(hexarr[b & 0x0f]);
}

void putstr(const char *str)
{
    char p;

    for (p = *str; p != 0; ++str, p = *str)
        putchar(p);
}
