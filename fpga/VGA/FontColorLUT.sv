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

`ifndef FONT_ROM_PATH
`define FONT_ROM_PATH "."
`endif

`default_nettype none
module FontColorLUT(input logic clk,
                    input logic render_cursor,
                    input logic [7:0] glyph,
                    input logic [2:0] glyph_row,
                    input logic [2:0] glyph_col,
                    input logic [3:0] foreground,
                    input logic [3:0] background,
                    input logic graphics_enabled,
                    input logic [7:0] graphics_colour,
                    input logic vga_256_color,
                    // verilator lint_off UNUSED
                    input logic [17:0] vga_dac_rd,
                    // verilator lint_on UNUSED
                    input logic bright_colors,
                    input logic palette_sel,
                    input logic [3:0] background_color,
                    output logic [3:0] r,
                    output logic [3:0] g,
                    output logic [3:0] b);

// 64 bits per character, 256 characters
logic font_rom[16384];
initial $readmemb({{`FONT_ROM_PATH, "/font.bin"}}, font_rom);

logic font_bit;

wire [13:0] font_mem_addr = {glyph, glyph_row[2:0], glyph_col[2:0]};

reg [11:0] foreground_rgb;
reg [11:0] background_rgb;
reg [11:0] graphics_pixel_colour;

wire [3:0] dac_r = vga_dac_rd[17:14];
wire [3:0] dac_g = vga_dac_rd[11:8];
wire [3:0] dac_b = vga_dac_rd[5:2];

reg [11:0] text_color_lut [0:15] = '{
    12'h0_0_0, // black
    12'h0_0_a, // blue
    12'h0_a_0, // green
    12'h0_a_a, // cyan
    12'ha_0_0, // red
    12'ha_0_a, // magenta
    12'ha_5_0, // brown
    12'ha_a_a, // white
    12'h5_5_5, // gray
    12'h5_5_f, // bright blue
    12'h5_f_5, // bright green
    12'h5_f_f, // bright cyan
    12'hf_5_5, // bright red
    12'hf_5_f, // bright magenta
    12'hf_f_5, // yellow
    12'hf_f_f  // bright white
};

reg [11:0] graphics_color_lut [0:15] = '{
    12'h0_0_0, // black
    12'h0_a_0, // green
    12'ha_0_0, // red
    12'ha_5_0, // brown
    12'h0_0_0, // black
    12'h5_f_5, // bright green
    12'hf_5_5, // bright red
    12'hf_f_5,  // yellow
    // Alternate palette
    12'h0_0_0, // black
    12'h0_a_a, // cyan
    12'ha_0_a, // magenta
    12'ha_a_a, // white
    12'h0_0_0, // black
    12'h5_f_f, // bright cyan
    12'hf_5_f, // bright magenta
    12'hf_f_f  // bright white
};

wire [11:0] graphics_pixel_color_int;

always_comb begin
    if (vga_256_color) begin
        graphics_pixel_color_int = {dac_r, dac_g, dac_b};
    end else begin
        if (~|graphics_colour)
            graphics_pixel_color_int = text_color_lut[background_color];
        else
            graphics_pixel_color_int =
                graphics_color_lut[{palette_sel, bright_colors, graphics_colour[1:0]}];
    end
end

always_ff @(posedge clk) begin
    foreground_rgb <= text_color_lut[foreground];
    background_rgb <= text_color_lut[background];
    graphics_pixel_colour <= graphics_pixel_color_int;
end

assign {r, g, b} = graphics_enabled ?
    graphics_pixel_colour :
    (font_bit ^ render_cursor ? foreground_rgb : background_rgb);

always_ff @(posedge clk)
    font_bit <= font_rom[font_mem_addr];
    
endmodule
