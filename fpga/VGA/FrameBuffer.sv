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

module FrameBuffer(input logic clk,
                   input logic sys_clk,
                   input logic is_border,
                   // CPU port
                   input logic cs,
                   input logic data_m_access,
                   output logic data_m_ack,
                   input logic [19:1] data_m_addr,
                   input logic data_m_wr_en,
                   input logic [15:0] data_m_data_in,
                   output logic [15:0] data_m_data_out,
                   input logic [1:0] data_m_bytesel,
                   // VGA signals
                   input logic [2:0] glyph_row,
                   input logic is_blank,
                   input logic cursor_enabled,
                   input logic [14:0] cursor_pos,
                   input logic [2:0] cursor_scan_start,
                   input logic [2:0] cursor_scan_end,
                   input logic [10:0] address,
                   output logic [7:0] glyph,
                   output logic [3:0] background,
                   output logic [3:0] foreground,
                   output logic render_cursor);

wire [15:0] cpu_q;
wire cpu_wr_en = data_m_access & cs & data_m_wr_en;
assign data_m_data_out = data_m_ack ? cpu_q : 16'b0;

logic vga_valid;
wire [15:0] vga_q;
assign {background, foreground, glyph} = is_border || !vga_valid ? 16'b0 : vga_q;

always_ff @(posedge clk)
    render_cursor <= ~is_blank && cursor_enabled && address == cursor_pos[10:0] &&
        glyph_row >= cursor_scan_start && glyph_row <= cursor_scan_end;

FrameBufferRAM FrameBufferRAM(.clock_a(sys_clk),
                              // CPU
                              .address_a(data_m_addr[11:1]),
                              .byteena_a(data_m_bytesel),
                              .data_a(data_m_data_in),
                              .q_a(cpu_q),
                              .wren_a(cpu_wr_en),
                              // VGA
                              .clock_b(clk),
                              .address_b(address),
                              .data_b(16'b0),
                              .q_b(vga_q),
                              .wren_b(1'b0));

always_ff @(posedge sys_clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk)
    vga_valid <= ~is_blank;

endmodule
