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
module TempReg(input logic clk,
               input logic reset,
               input logic [15:0] wr_val,
               input logic wr_en,
               output logic [15:0] val);

reg [15:0] r;

assign val = r;

always @(posedge clk or posedge reset)
    if (reset)
        r <= 16'b0;
    else if (wr_en)
        r <= wr_val;

endmodule
