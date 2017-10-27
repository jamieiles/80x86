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

task do_mul;
    output [31:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;
    input is_signed;

    begin
        flags_out = flags_in;
        flags_out[ZF_IDX] = 1'b0;
        out = 32'b0;
        if (!is_8_bit) begin
            if (is_signed)
                out[31:0] = {{16{a[15]}}, a} * {{16{b[15]}}, b};
            else
                out[31:0] = {16'b0, a} * {16'b0, b};
            flags_out[CF_IDX] = |out[31:16];
            flags_out[OF_IDX] = |out[31:16];
        end else begin
            if (is_signed)
                out = {16'b0, {{8{a[7]}}, a[7:0]} * {{8{b[7]}}, b[7:0]}};
            else
                out = {16'b0, {8'b0, a[7:0]} * {8'b0, b[7:0]}};
            flags_out[CF_IDX] = |out[15:8];
            flags_out[OF_IDX] = |out[15:8];
        end
    end
endtask
