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
module JumpTest(input logic [7:0] opcode,
                // verilator lint_off UNUSED
                input logic [15:0] flags,
                // verilator lint_on UNUSED
                output logic taken);

always_comb begin
    taken = 1'b0;

    unique casez(opcode)
    8'b01100010: taken = flags[CF_IDX]; // BOUND
    8'b01110000: taken = flags[OF_IDX]; // JO
    8'b01110001: taken = ~flags[OF_IDX]; // JNO
    8'b01110010: taken = flags[CF_IDX]; // JB
    8'b01110011: taken = ~flags[CF_IDX]; // JNB
    8'b01110100: taken = flags[ZF_IDX]; // JE
    8'b01110101: taken = ~flags[ZF_IDX]; // JNE
    8'b01110110: taken = flags[CF_IDX] | flags[ZF_IDX]; // JBE
    8'b01110111: taken = ~(flags[CF_IDX] | flags[ZF_IDX]); // JNBE
    8'b01111000: taken = flags[SF_IDX]; // JS
    8'b01111001: taken = ~flags[SF_IDX]; // JNS
    8'b01111010: taken = flags[PF_IDX]; // JP
    8'b01111011: taken = ~flags[PF_IDX]; // JNP
    8'b01111100: taken = flags[OF_IDX] ^ flags[SF_IDX]; // JL
    8'b01111101: taken = ~(flags[SF_IDX] ^ flags[OF_IDX]); // JNL
    8'b01111110: taken = (flags[OF_IDX] ^ flags[SF_IDX]) | flags[ZF_IDX]; // JLE
    8'b01111111: taken = ~((flags[SF_IDX] ^ flags[OF_IDX]) | flags[ZF_IDX]); // JNLE
    8'b11001110: taken = flags[OF_IDX]; // INTO
    default: taken = 1'b0;
    endcase
end

endmodule
