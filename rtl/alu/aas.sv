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

task do_aas;
    output [15:0] out;
    input [15:0] a;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        out = a;
        if (a[3:0] > 4'd9 || flags_in[AF_IDX]) begin
            out[3:0] -= 4'd6;
            out[7:4] = 4'b0;
            out[15:8] -= 8'b1;
            flags_out[CF_IDX] = 1'b1;
            flags_out[AF_IDX] = 1'b1;
        end else begin
            flags_out[CF_IDX] = 1'b0;
            flags_out[AF_IDX] = 1'b0;
        end
    end
endtask
