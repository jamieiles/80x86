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

`default_nettype none
module VGAController(input logic clk,
                     input logic sys_clk,
		     input logic reset,
                     output logic fb_access,
                     output logic [15:0] fb_address,
                     input logic fb_ack,
                     input logic [15:0] fb_data,
                     // VGA signals
		     output logic vga_hsync,
		     output logic vga_vsync,
		     output logic [3:0] vga_r,
		     output logic [3:0] vga_g,
		     output logic [3:0] vga_b,
                     input logic graphics_enabled,
                     input logic cursor_enabled,
                     input logic bright_colors,
                     input logic palette_sel,
                     input logic [3:0] background_color,
                     input logic [14:0] cursor_pos,
                     input logic [2:0] cursor_scan_start,
                     input logic [2:0] cursor_scan_end,
                     input logic vga_256_color,
                     output logic [7:0] vga_dac_idx,
                     input logic [17:0] vga_dac_rd);

wire hsync;
wire vsync;
wire is_blank;
wire [9:0] row;
wire [9:0] col;
logic fdata;

// Putting a 640x400 display into a 640x480 screen.  Black bars of 40 pixels
// at the top and bottom.
wire [9:0] shifted_row = is_border ? 10'b0 : row - 10'd40;
wire is_border = row < 10'd40 || row >= 10'd440;

wire [3:0] fb_background;
wire [3:0] fb_foreground;
wire [7:0] fb_glyph;
logic [2:0] fb_fcl_col;
logic [2:0] fb_fcl_row;
wire render_cursor;
wire [7:0] graphics_colour;

logic [2:0] vsync_pipe;
assign vga_vsync = vsync_pipe[0];
logic [2:0] hsync_pipe;
assign vga_hsync = hsync_pipe[0];

assign vga_dac_idx = graphics_colour;

VGASync VGASync(.*);

FrameBuffer FrameBuffer(.glyph(fb_glyph),
                        .background(fb_background),
                        .foreground(fb_foreground),
                        .row(shifted_row),
			.*);

FontColorLUT FontColorLUT(.glyph(fb_glyph),
                          .glyph_row(fb_fcl_row),
                          .glyph_col(fb_fcl_col),
                          .foreground(fb_foreground),
                          .background(fb_background),
                          .r(vga_r),
                          .g(vga_g),
                          .b(vga_b),
			  .*);

always_ff @(posedge clk) begin
    fb_fcl_col <= col[2:0];
    fb_fcl_row <= shifted_row[3:1];
end

always_ff @(posedge clk) begin
    vsync_pipe <= {vsync, vsync_pipe[2:1]};
    hsync_pipe <= {hsync, hsync_pipe[2:1]};
end

endmodule
