#pragma once

enum led_id {
    LED_DISK = (1 << 0),
    LED_KBD_BUFFER_NON_EMPTY = (1 << 1),
    LED_KBD_BUFFER_FULL = (1 << 2),
    LED_PANIC = (1 << 3),
};

void led_set(enum led_id led);
void led_clear(enum led_id led);
