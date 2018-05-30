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
module FrameBuffer(input logic clk,
                   input logic sys_clk,
                   input logic reset,
                   input logic is_border,
                   // CPU port
                   output logic fb_access,
                   output logic [15:0] fb_address,
                   input logic fb_ack,
                   input logic [15:0] fb_data,
                   // VGA signals
                   input logic [9:0] row,
                   input logic [9:0] col,
                   input logic is_blank,
                   input logic cursor_enabled,
                   input logic [14:0] cursor_pos,
                   input logic [2:0] cursor_scan_start,
                   input logic [2:0] cursor_scan_end,
                   input logic graphics_enabled,
                   output logic [7:0] glyph,
                   output logic [3:0] background,
                   output logic [3:0] foreground,
                   output logic render_cursor,
                   input logic vga_256_color,
                   output logic [7:0] graphics_colour);

logic vga_valid;
wire [15:0] vga_q;
assign {background, foreground, glyph} = is_border || !vga_valid ? 16'b0 : vga_q;

// 2 vertical pixels per horizontal pixel to scale out.
wire [11:0] text_address = {1'b0, ({1'b0, row} / 11'd16) * 11'd80 + ({1'b0, col} / 11'd8)};
// Double pixels for graphics mode.
wire [2:0] glyph_row = row[3:1];

wire [12:0] graphics_row = {4'b0, row[9:1]};
wire [12:0] graphics_col = {4'b0, col[9:1]};

reg [2:0] pixel_word_offs;

always_ff @(posedge clk)
    pixel_word_offs <= graphics_col[2:0];

always_comb begin
    if (!vga_256_color) begin
        case (pixel_word_offs)
        3'b011: graphics_colour = {6'b0, vga_q[1:0]};
        3'b010: graphics_colour = {6'b0, vga_q[3:2]};
        3'b001: graphics_colour = {6'b0, vga_q[5:4]};
        3'b000: graphics_colour = {6'b0, vga_q[7:6]};
        3'b111: graphics_colour = {6'b0, vga_q[9:8]};
        3'b110: graphics_colour = {6'b0, vga_q[11:10]};
        3'b101: graphics_colour = {6'b0, vga_q[13:12]};
        3'b100: graphics_colour = {6'b0, vga_q[15:14]};
        endcase
    end else
        graphics_colour = pixel_word_offs[0] ? vga_q[15:8] : vga_q[7:0];

    if (is_border || !vga_valid)
        graphics_colour = 8'b00;
end

always_ff @(posedge clk)
    render_cursor <= ~is_blank && cursor_enabled && text_address[10:0] == cursor_pos[10:0] &&
        glyph_row >= cursor_scan_start && glyph_row <= cursor_scan_end;

FBPrefetch FBPrefetch(.sys_clk(sys_clk),
                      .vga_clk(clk),
                      .mode(get_video_mode(graphics_enabled,
                                           vga_256_color)),
                      .q(vga_q),
                      .*);

always_ff @(posedge clk)
    vga_valid <= ~is_blank;

endmodule
