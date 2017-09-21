#include "leds.h"
#include "bios.h"
#include "io.h"

#define LED_PORT 0xfffe

void led_set(enum led_id led)
{
    outw(LED_PORT, inw(LED_PORT) | led);
}

void led_clear(enum led_id led)
{
    outw(LED_PORT, inw(LED_PORT) & ~led);
}
