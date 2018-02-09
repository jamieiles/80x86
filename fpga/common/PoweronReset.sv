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
module PoweronReset(input logic sys_clk,
                    input logic pll_locked,
                    output logic poweron_reset);

initial poweron_reset = 1'b1;

localparam reset_counter_bits = $clog2(5000);
reg [reset_counter_bits-1:0] reset_counter;
wire sys_pll_locked;

BitSync         PLLLockedSync(.clk(sys_clk),
                              .d(pll_locked),
                              .q(sys_pll_locked));

always_ff @(posedge sys_clk) begin
    if (sys_pll_locked) begin
        reset_counter <= reset_counter - 1'b1;
        if (~|reset_counter)
            poweron_reset <= 1'b0;
    end else if (poweron_reset)
        reset_counter <= {reset_counter_bits{1'b1}};
end

endmodule
