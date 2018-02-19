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

`default_nettype none
module BlockRam(input logic clk,
                // Port A
                input logic [addr_bits-1:0] addr_a,
                input logic wr_en_a,
                input logic [1:0] be_a,
                input logic [15:0] wdata_a,
                output logic [15:0] q_a,
                // Port B
                input logic [addr_bits-1:0] addr_b,
                input logic wr_en_b,
                input logic [1:0] be_b,
                input logic [15:0] wdata_b,
                output logic [15:0] q_b);

parameter words = 8;
localparam addr_bits = $clog2(words);

logic [1:0][7:0] ram[0:words-1] /* synthesis syn_ramstyle = "no_rw_check"*/;

logic [15:0] r_a, r_b, bypass_a_val, bypass_b_val;
logic bypass_a, bypass_b;

assign q_a = bypass_a ? bypass_a_val : r_a;
assign q_b = bypass_b ? bypass_b_val : r_b;

always_ff @(posedge clk) begin
    bypass_a <= addr_a == addr_b && wr_en_b;
    bypass_b <= addr_a == addr_b && wr_en_a;
    bypass_a_val <= wdata_b;
    bypass_b_val <= wdata_a;
end

always_ff @(posedge clk) begin
    if (wr_en_a) begin
        if (be_a[0])
            ram[addr_a][0] <= wdata_a[7:0];
        if (be_a[1])
            ram[addr_a][1] <= wdata_a[15:8];
    end
    r_a <= ram[addr_a];
end

always_ff @(posedge clk) begin
    if (wr_en_b) begin
        if (be_b[0])
            ram[addr_b][0] <= wdata_b[7:0];
        if (be_b[1])
            ram[addr_b][1] <= wdata_b[15:8];
    end
    r_b <= ram[addr_b];
end

endmodule
