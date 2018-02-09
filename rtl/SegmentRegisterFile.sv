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
module SegmentRegisterFile(input logic clk,
                           input logic reset,
                           // Read port.
                           input logic [1:0] rd_sel,
                           output logic [15:0] rd_val,
                           // Write port.
                           input logic wr_en,
                           input logic [1:0] wr_sel,
                           input logic [15:0] wr_val,
                           // CS port.
                           output logic [15:0] cs);

reg [15:0] registers[4];

wire rd_bypass = wr_en && wr_sel == rd_sel;

assign cs = registers[CS];

always_ff @(posedge reset)
    ; // Reset is handled by the microcode

always_ff @(posedge clk) begin
    if (wr_en)
        registers[wr_sel] <= wr_val;
end

always_ff @(posedge clk)
    rd_val <= rd_bypass ? wr_val : registers[rd_sel];

endmodule
