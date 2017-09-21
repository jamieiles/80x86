#include "leds.h"
#include "utils.h"

#include "serial.h"

void *memset(void *s, int c, unsigned short n)
{
    unsigned char *p = s;
    unsigned short m;

    for (m = 0; m < n; ++m)
        p[m] = c;

    return s;
}

void *memcpy(void *d, const void *s, unsigned short n)
{
    const unsigned char *s_p = s;
    unsigned char *d_p = d;
    unsigned short m;

    for (m = 0; m < n; ++m)
        d_p[m] = s_p[m];

    return d;
}

void panic(const char *p)
{
    led_set(LED_PANIC);

    putstr(p);
    putstr("\n\nHalted.\n");

    for (;;)
        continue;
}
