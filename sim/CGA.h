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

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "CPU.h"
#include "Cursor.h"
#include "Display.h"

static const GraphicsPalette cga_alt_palette = {{
    {0, 0, 0},          // black
    {0x00, 0xaa, 0xaa}, // cyan
    {0xaa, 0x00, 0xaa}, // magenta
    {0xaa, 0xaa, 0xaa}, // white
}};

static const GraphicsPalette cga_alt_bright_palette = {{
    {0, 0, 0},          // black
    {0x55, 0xff, 0xff}, // bright cyan
    {0xff, 0x55, 0xff}, // bright magenta
    {0xff, 0xff, 0xff}, // bright white
}};

static const GraphicsPalette cga_default_palette = {{
    {0, 0, 0},          // black
    {0x00, 0xaa, 0x00}, // green
    {0xaa, 0x00, 0x00}, // red
    {0xaa, 0x55, 0x00}, // brown
}};

static const GraphicsPalette cga_default_bright_palette = {{
    {0, 0, 0},          // black
    {0x55, 0xff, 0x55}, // bright green
    {0xff, 0x55, 0x55}, // bright red
    {0xff, 0xff, 0x55}, // yellow
}};

static const GraphicsPalette *color_palettes[] = {
    &cga_default_palette, &cga_alt_bright_palette, &cga_alt_palette,
    &cga_alt_bright_palette,
};

class CGA : public IOPorts
{
private:
    enum CRTCRegs {
        CURSOR_SCAN_START = 0xa,
        CURSOR_SCAN_END = 0xb,
        CURSOR_HIGH = 0xe,
        CURSOR_LOW = 0xf,
    };

public:
    const phys_addr buffer_phys = 0xb8000;
    const phys_addr vga_buffer_phys = 0xa0000;

    explicit CGA(Memory *mem)
        : IOPorts(0x03b4, 128),
          mem(mem),
          reg_idx(0),
          status(0),
          mode_reg(0),
          dac_wr_sel(0),
          dac_seq(0)
    {
        memset(idx_regs, 0, sizeof(idx_regs));
        display.set_graphics_palette(cga_default_palette);
    }

    void write8(uint16_t port_num, unsigned offs, uint8_t v)
    {
        if (port_num >= 0x20)
            port_num -= 0x20;

        if (port_num == 0 && offs == 0)
            reg_idx = v;
        else if (port_num == 0 && offs == 1)
            idx_regs[reg_idx] = v;
        else if (port_num == 4 && offs == 0)
            mode_reg = v & 0xb;
        else if (port_num == 4 && offs == 1)
            write_color_control(v);
        else if (port_num == 0x0c && offs == 0)
            attribute_mode_control = v;
        else if (port_num == 0x14 && offs == 0)
            dac_wr_sel = v;
        else if (port_num == 0x14 && offs == 1)
            write_dac(v);
    }

    uint8_t read8(uint16_t port_num, unsigned offs)
    {
        if (port_num >= 0x20)
            port_num -= 0x20;

        if (port_num == 0)
            return offs == 0 ? reg_idx : idx_regs[reg_idx];
        else if (port_num == 4 && offs == 0)
            return mode_reg;
        else if (port_num == 0xc && offs == 0)
            return attribute_mode_control;
        else if (port_num == 6 && offs == 0)
            return read_status();

        return 0;
    }

    void update();

private:
    Cursor get_cursor() const
    {
        auto cursor_loc = (static_cast<uint16_t>(idx_regs[CURSOR_HIGH]) << 8) |
                          idx_regs[CURSOR_LOW];
        auto cursor_row = cursor_loc / 80;
        auto cursor_col = cursor_loc % 80;
        auto cursor_scan_start = idx_regs[CURSOR_SCAN_START] & 0xf;
        auto cursor_scan_end = idx_regs[CURSOR_SCAN_END] & 0xf;
        auto cursor_enabled = (idx_regs[CURSOR_SCAN_START] & 0x30) != 0x10;

        return Cursor(cursor_row * 8 + (cursor_scan_start),
                      cursor_row * 8 + (cursor_scan_end), cursor_col * 8,
                      cursor_col * 8 + 8, cursor_enabled);
    }

    bool is_graphics() const
    {
        return mode_reg & (1 << 1);
    }

    bool is_graphics_256() const
    {
        return attribute_mode_control == 0x41;
    }

    void write_dac(uint8_t v)
    {
        if (dac_seq == 0)
            dac_palette.colors[dac_wr_sel].r = ((v & 0x3f) << 2) & 0xf0;
        if (dac_seq == 1)
            dac_palette.colors[dac_wr_sel].g = ((v & 0x3f) << 2) & 0xf0;
        if (dac_seq == 2)
            dac_palette.colors[dac_wr_sel].b = ((v & 0x3f) << 2) & 0xf0;
        dac_seq = (dac_seq + 1) % 3;
        if (dac_seq == 0) {
            ++dac_wr_sel;
            display.set_graphics_palette(dac_palette);
        }
    }

    void write_color_control(uint8_t v)
    {
        auto palette_idx = (v >> 4) & 0x3;
        auto palette = *color_palettes[palette_idx];

        palette.colors[0] = cga_full_palette[v & 0x0f];

        display.set_graphics_palette(palette);
    }

    uint8_t read_status()
    {
        static int c;
        if (c % 16 == 0)
            status ^= 0x9;
        ++c;
        return status;
    }

    void update_text();
    void update_graphics();
    void update_graphics_256();

    Memory *mem;
    Display display;
    uint8_t reg_idx;
    uint8_t idx_regs[256];
    uint8_t status;
    uint8_t mode_reg;
    uint8_t attribute_mode_control;
    GraphicsPalette dac_palette;
    uint8_t dac_wr_sel;
    uint8_t dac_seq;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & mem;
        ar & reg_idx;
        ar & idx_regs;
        ar & status;
        ar & mode_reg;
        ar & attribute_mode_control;
        ar & dac_palette;
        ar & dac_wr_sel;
        ar & dac_seq;

        if (is_graphics_256())
            display.set_graphics_palette(dac_palette);
        // clang-format on
    }
};

void CGA::update()
{
    display.set_graphics(is_graphics());

    if (is_graphics_256())
        update_graphics_256();
    else if (is_graphics())
        update_graphics();
    else
        update_text();
}

void CGA::update_graphics()
{
    int cols = 320;
    int rows = 200;

    for (auto row = 0; row < rows; ++row) {
        for (auto col = 0; col < cols; ++col) {
            // 4 pixels per byte
            auto pixel_mem_address = ((row / 2) * cols + col) / 4;
            // Interlaced
            if (row % 2)
                pixel_mem_address += 8192;

            auto pixel_shift = (3 - (col % 4)) * 2;

            auto pixel = mem->read<uint8_t>(buffer_phys + pixel_mem_address);
            pixel >>= pixel_shift;
            pixel &= 0x3;

            display.set_pixel(row, col, pixel);
        }
    }

    display.refresh();
}

void CGA::update_graphics_256()
{
    int cols = 320;
    int rows = 200;

    for (auto row = 0; row < rows; ++row) {
        for (auto col = 0; col < cols; ++col) {
            auto pixel_mem_address = row * cols + col;
            auto pixel =
                mem->read<uint8_t>(vga_buffer_phys + pixel_mem_address);
            display.set_pixel(row, col, pixel);
        }
    }

    display.refresh();
}

void CGA::update_text()
{
    int cols = 80;
    int rows = 25;

    for (auto row = 0; row < rows; ++row) {
        for (auto col = 0; col < cols; ++col) {
            auto addr = buffer_phys + ((row * cols) + col) * sizeof(uint16_t);
            auto char_attr = mem->read<uint16_t>(addr);

            display.set_cursor(row, col);
            display.write_char(char_attr);
        }
    }

    display.refresh(get_cursor());
}
