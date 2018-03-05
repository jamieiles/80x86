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
module SegmentOverride(input logic clk,
                       input logic reset,
                       input logic next_instruction,
                       input logic flush,
                       input logic update,
                       input logic force_segment,
                       input logic bp_is_base,
                       input logic segment_override,
                       input logic [1:0] override_in,
                       input logic [1:0] microcode_sr_rd_sel,
                       output logic [1:0] sr_rd_sel);

reg [1:0] override;
reg override_active;

always_comb begin
    if (force_segment)
        sr_rd_sel = microcode_sr_rd_sel;
    else if (update && segment_override)
        sr_rd_sel = override_in;
    else if (override_active)
        sr_rd_sel = override;
    else if (bp_is_base)
        sr_rd_sel = SS;
    else
        sr_rd_sel = microcode_sr_rd_sel;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        override_active <= 1'b0;
        override <= 2'b00;
    end else begin
        if (next_instruction || flush) begin
            override_active <= 1'b0;
            override <= 2'b00;
        end
        if (update && segment_override) begin
            override <= override_in;
            override_active <= 1'b1;
        end
    end
end

endmodule
