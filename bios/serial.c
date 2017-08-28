#include "bios.h"
#include "bda.h"
#include "io.h"

extern void video_putchar(char c);

#define UART_DATA_PORT 0xfffa
#define UART_STATUS_PORT 0xfffb
#define UART_TX_BUSY 0x02
#define UART_RX_READY 0x01

void putchar(unsigned char c)
{
    outb(UART_DATA_PORT, c);

    while (inb(UART_STATUS_PORT) & UART_TX_BUSY)
        continue;

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
        putchar('\r');
}

int serial_poll(void)
{
    int ready = inb(UART_STATUS_PORT) & UART_RX_READY;

    if (ready) {
        bda_write(kbd_buffer[0], inb(UART_DATA_PORT));
        bda_write(kbd_buffer_tail, 0x20);
    }

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
