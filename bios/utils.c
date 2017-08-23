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

void panic(const char *p)
{
    putstr(p);
    putstr("\n\r\n\rHalted.\n\r");

    for (;;)
        continue;
}
