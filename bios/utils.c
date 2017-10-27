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
