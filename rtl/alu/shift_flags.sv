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

task shift_flags;
    inout [15:0] flags;
    input is_8_bit;
    // verilator lint_off UNUSED
    input [15:0] out;
    input [15:0] a;
    // verilator lint_on UNUSED

    begin
        flags[PF_IDX] = ~^out[7:0];
        flags[SF_IDX] = out[is_8_bit ? 7 : 15];
        flags[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;
    end
endtask
