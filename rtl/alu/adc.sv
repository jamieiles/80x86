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

task do_adc;
    output [15:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        if (!is_8_bit) begin
            {flags_out[CF_IDX], out} = a + b + {15'b0, flags_in[CF_IDX]};
            flags_out[OF_IDX] = ~(a[15] ^ b[15]) & (out[15] ^ b[15]);
        end else begin
            out = a + b + {15'b0, flags_in[CF_IDX]};
            flags_out[CF_IDX] = a[8] ^ b[8] ^ out[8];
            flags_out[OF_IDX] = ~(a[7] ^ b[7]) & (out[7] ^ b[7]);
        end
        common_flags(flags_out, is_8_bit, out, a, b);
    end
endtask
