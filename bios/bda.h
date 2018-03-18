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

struct __attribute__((packed)) bios_data_area {
    unsigned short com_addresses[4];
    unsigned short lpt_addresses[4];
    unsigned short equipment_list;
    unsigned char ir_kbd_error_count;
    unsigned short mem_kbytes;
    unsigned char reserved1;
    unsigned char ps2_bios_control;
    unsigned char keyboard_flags[2];
    unsigned char keypad_entry;
    unsigned short kbd_buffer_head;
    unsigned short kbd_buffer_tail;
    unsigned short kbd_buffer[16];
    unsigned char drive_recalibration_status;
    unsigned char diskette_motor_status;
    unsigned char motor_shutoff_count;
    unsigned char diskette_status;
    unsigned char nec_diskette_motor_status[7];
    unsigned char video_mode;
    unsigned short num_screen_cols;
    unsigned short video_regen_buffer_bytes;
    unsigned short video_regen_offset;
    unsigned short cursor_offsets[8];
    unsigned char cursor_end;
    unsigned char cursor_start;
    unsigned char active_page;
    unsigned short crt_controller_base;
    unsigned char crt_mode_control;
    unsigned char cga_pallette_mask;
    unsigned char cassette_take_ctrl[5];
    unsigned short timer_counter_low;
    unsigned short timer_counter_high;
    unsigned char clock_rollover;
    unsigned char break_hit;
    unsigned short soft_reset_flag;
    unsigned char hard_disk_status;
    unsigned char num_hard_disks;
    unsigned char fixed_disk_control;
    unsigned char fixed_disk_controller_port;
    unsigned long parallel_timeout;
    unsigned long serial_timeout;
    unsigned short keyboard_buffer_start;
    unsigned short keyboard_buffer_end;
    unsigned char last_screen_row;
    unsigned char char_point_height;
    unsigned char pcjr_repeat_delay;
    unsigned char video_mode_options;
    unsigned char ega_feature_bits;
    unsigned char video_display_data_area;
    unsigned char dcc_index;
};
