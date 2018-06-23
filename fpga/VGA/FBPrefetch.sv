// Copyright Jamie Iles, 2018
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

`include "VGATypes.sv"

module FBPrefetch(input logic sys_clk,
                  input logic vga_clk,
                  input logic reset,
                  input VideoMode_t mode,
                  input logic [9:0] row,
                  input logic [9:0] col,
                  output logic [15:0] fb_address,
                  output logic fb_access,
                  input logic fb_ack,
                  input logic [15:0] fb_data,
                  output logic [15:0] q);

localparam cols = 640;
localparam rows = 400;

wire [8:0] vga_address;
wire [8:0] sys_address = {sys_cur_buffer, words_done};

assign fb_access = words_done < words_per_row & ~fb_ack;

VGAPrefetchRAM VGAPrefetchRAM(.clock_a(sys_clk),
                              .address_a(sys_address),
                              .data_a(fb_data),
                              .wren_a(fb_ack),
                              .q_a(),
                              .clock_b(vga_clk),
                              .address_b(vga_address),
                              .data_b(16'b0),
                              .wren_b(1'b0),
                              .q_b(q));

logic vga_cur_buffer;
logic sys_cur_buffer;
logic do_next_row;
logic [7:0] words_per_row;
wire [9:0] sync_row;
VideoMode_t sync_mode;
logic [9:0] sys_next_row;
VideoMode_t sys_mode;
logic sys_load;

logic [7:0] words_done;
wire [9:0] next_row = row == rows - 1'b1 ? 10'b0 : row + 1'b1;
wire sync_ready;

wire [7:0] col_address = sys_load ? 8'b0 : words_done;
wire [9:0] row_base = sys_load ? sync_row : sys_next_row;
wire [12:0] graphics_row = {4'b0, row_base[9:1]};

MCP #(.width(12),
      .reset_val(12'hfff))
    RowSync(.reset(reset),
            .clk_a(vga_clk),
            .a_ready(sync_ready),
            .a_send(do_next_row & sync_ready),
            .a_datain({next_row, mode}),
            .clk_b(sys_clk),
            .b_data({sync_row, sync_mode}),
            .b_load(sys_load));

always_ff @(posedge sys_clk or posedge reset) begin
    if (reset) begin
        sys_next_row <= 8'b0;
        sys_mode <= VIDEO_MODE_TEXT;
    end else begin
        if (sys_load) begin
            sys_next_row <= sync_row;
            sys_mode <= sync_mode;
        end
    end
end

always_comb begin
    case (mode)
    VIDEO_MODE_TEXT:
        fb_address = {1'b0, ({1'b0, row_base} / 11'd16) * 11'd80 +
            {2'b0, col_address}};
    VIDEO_MODE_4_COLOR: begin
        fb_address = {3'b0, graphics_row[0], 12'b0} +
            ({6'b0, graphics_row[11:1]} * 16'd40) +
            {3'b0, col_address};
    end
    VIDEO_MODE_256_COLOR:
        fb_address = {7'b0, row_base[9:1]} * 16'd160 + col_address;
    default: fb_address = 16'b0;
    endcase
end

always_comb begin
    case (sys_mode)
    // 80 chars, 2 bytes per char (attr+val)
    VIDEO_MODE_TEXT: words_per_row = 8'd80;
    // 320 pixels, 2 bits per pixel, 8 pixels per word
    VIDEO_MODE_4_COLOR: words_per_row = 8'd40;
    // 320 pixels, 2 pixels per word
    VIDEO_MODE_256_COLOR: words_per_row = 8'd160;
    default: ;
    endcase
end

always_comb begin
    do_next_row = 1'b0;

    if (col == cols - 1'b1) begin
        case (mode)
        VIDEO_MODE_TEXT: do_next_row = &row[3:0];
        VIDEO_MODE_4_COLOR, VIDEO_MODE_256_COLOR: do_next_row = row[0];
        default: do_next_row = 1'b0;
        endcase
    end
end

always_ff @(posedge sys_clk or posedge reset)
    if (reset)
        words_done <= 8'b0;
    else begin
        if (fb_ack)
            words_done <= words_done + 1'b1;
        if (sys_load)
            words_done <= 8'b0;
    end

always_comb begin
    case (mode)
    VIDEO_MODE_TEXT: vga_cur_buffer = row[4];
    VIDEO_MODE_4_COLOR, VIDEO_MODE_256_COLOR: vga_cur_buffer = row[1];
    default: vga_cur_buffer = 1'b0;
    endcase
end

always_comb begin
    case (sys_mode)
    VIDEO_MODE_TEXT: sys_cur_buffer = sys_next_row[4];
    VIDEO_MODE_4_COLOR, VIDEO_MODE_256_COLOR: sys_cur_buffer = sys_next_row[1];
    default: sys_cur_buffer = 1'b0;
    endcase
end

always_comb begin
    case (mode)
    VIDEO_MODE_TEXT:
        vga_address = {vga_cur_buffer, 1'b0, col[9:3]};
    VIDEO_MODE_4_COLOR:
        vga_address = {vga_cur_buffer, 2'b0, col[9:4]};
    VIDEO_MODE_256_COLOR: vga_address = {vga_cur_buffer, col[9:2]};
    default: vga_address = 10'b0;
    endcase
end

endmodule
