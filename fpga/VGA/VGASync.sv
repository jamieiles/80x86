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
module VGASync(input logic clk,
               input logic reset,
               output logic vsync,
               output logic hsync,
               output logic is_blank,
               output logic [9:0] row,
               output logic [9:0] col);

reg [9:0] hcount;
reg [9:0] vcount;

localparam v_front_porch = 10'd10;
localparam v_back_porch = 10'd33;
localparam v_sync_count = 10'd2;
localparam v_lines = 10'd480;
localparam v_total = v_front_porch + v_back_porch + v_sync_count + v_lines;

localparam h_front_porch = 10'd16;
localparam h_back_porch = 10'd48;
localparam h_sync_count = 10'd96;
localparam h_lines = 10'd640;
localparam h_total = h_front_porch + h_back_porch + h_sync_count + h_lines;

wire v_blank = vcount < (v_sync_count + v_back_porch) ||
               vcount >= (v_sync_count + v_back_porch + v_lines);
wire h_blank = hcount < (h_sync_count + h_back_porch) ||
               hcount >= (h_sync_count + h_back_porch + h_lines);
assign is_blank = v_blank | h_blank;

assign vsync = ~(vcount < v_sync_count);
assign hsync = ~(hcount < h_sync_count);

assign row = vcount - (v_sync_count + v_back_porch);
assign col = is_blank ? 10'b0 : hcount - (h_sync_count + h_back_porch);

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        hcount <= 10'b0;
        vcount <= 10'b0;
    end else begin
        hcount <= hcount == h_total - 1'b1 ? 10'd0 : hcount + 1'b1;
        if (hcount == h_total - 1'b1)
            vcount <= vcount == v_total - 1'b1 ? 10'd0 : vcount + 1'b1;
    end
end

endmodule
