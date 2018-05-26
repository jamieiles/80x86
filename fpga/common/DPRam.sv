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
module DPRam(input logic clk,
                // Port A
                input logic [addr_bits-1:0] addr_a,
                input logic wr_en_a,
                input logic [width-1:0] wdata_a,
                output logic [width-1:0] q_a,
                // Port B
                input logic [addr_bits-1:0] addr_b,
                input logic wr_en_b,
                input logic [width-1:0] wdata_b,
                output logic [width-1:0] q_b);

parameter words = 8;
parameter width = 1;
localparam addr_bits = $clog2(words);

`ifdef FORCE_M9K
logic [width-1:0] ram[0:words-1] /* synthesis syn_ramstyle = "no_rw_check,M9K"*/;
`else
logic [width-1:0] ram[0:words-1] /* synthesis syn_ramstyle = "no_rw_check"*/;
`endif

logic [width-1:0] r_a, r_b, bypass_a_val, bypass_b_val;
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
    if (wr_en_a)
        ram[addr_a] <= wdata_a;
    r_a <= ram[addr_a];
end

always_ff @(posedge clk) begin
    if (wr_en_b)
        ram[addr_b] <= wdata_b;
    r_b <= ram[addr_b];
end

endmodule
