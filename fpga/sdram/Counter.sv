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
module Counter #(parameter count_width = 0,
                 parameter count_max = 255)
                (input logic clk,
                 input logic reset,
                 output logic [count_width - 1:0] count);

reg [count_width - 1:0] count_reg;

always @(posedge clk) begin
	if (reset)
		count_reg <= 0;
	else if (count_reg != count_max[count_width - 1:0])
		count_reg <= count_reg + 1'b1;
end

assign count = count_reg;

endmodule
