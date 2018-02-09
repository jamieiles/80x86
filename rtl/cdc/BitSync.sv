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
module BitSync(input logic clk,
               input logic reset,
               input logic d,
               output logic q);

`ifdef verilator

reg p1;
reg p2;

assign q = p2;

always_ff @(posedge clk or posedge reset)
    if (reset)
        p1 <= 1'b0;
    else
        p1 <= d;

always_ff @(posedge clk or posedge reset)
    if (reset)
        p2 <= 1'b0;
    else
        p2 <= p1;

`else

altera_std_synchronizer sync(.clk(clk),
                             .reset_n(~reset),
                             .din(d),
                             .dout(q));

`endif

endmodule
