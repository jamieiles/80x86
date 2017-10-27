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

module FontColorLUT(input logic clk,
                    input logic render_cursor,
                    input logic [7:0] glyph,
                    input logic [2:0] glyph_row,
                    input logic [2:0] glyph_col,
                    input logic [3:0] foreground,
                    input logic [3:0] background,
                    output logic [3:0] r,
                    output logic [3:0] g,
                    output logic [3:0] b);

// 64 bits per character, 256 characters
logic font_rom[16384];
initial $readmemb({{`FONT_ROM_PATH, "/font.bin"}}, font_rom);

logic font_bit;

wire [13:0] font_mem_addr = {glyph, glyph_row[2:0], glyph_col[2:0]};

wire [11:0] foreground_rgb;
wire [11:0] background_rgb;

reg [11:0] color_lut [0:15] = '{
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

always_ff @(posedge clk) begin
    foreground_rgb <= color_lut[foreground];
    background_rgb <= color_lut[background];
end

assign {r, g, b} = font_bit ^ render_cursor ? foreground_rgb : background_rgb;

always_ff @(posedge clk)
    font_bit <= font_rom[font_mem_addr];
    
endmodule
