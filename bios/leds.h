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

enum led_id {
    LED_DISK = (1 << 0),
    LED_KBD_BUFFER_NON_EMPTY = (1 << 1),
    LED_KBD_BUFFER_FULL = (1 << 2),
    LED_PANIC = (1 << 3),
};

void led_set(enum led_id led);
void led_clear(enum led_id led);
