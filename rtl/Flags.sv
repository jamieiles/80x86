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
module Flags(input logic clk,
             input logic reset,
             // verilator lint_off UNUSED
             input logic [15:0] flags_in,
             // verilator lint_on UNUSED
             input logic [8:0] update_flags,
             output logic [15:0] flags_out);

reg C, P, A, Z, S, T, I, D, O;

generate
if (`S80X86_PSEUDO_286_INT == 1)
    assign flags_out = {4'b0000, O, D, I, T, S, Z, 1'b0, A, 1'b0, P, 1'b1, C};
else
    assign flags_out = {4'b1111, O, D, I, T, S, Z, 1'b0, A, 1'b0, P, 1'b1, C};
endgenerate

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        C <= 1'b0;
        P <= 1'b0;
        A <= 1'b0;
        Z <= 1'b0;
        S <= 1'b0;
        T <= 1'b0;
        I <= 1'b0;
        D <= 1'b0;
        O <= 1'b0;
    end else begin
        C <= update_flags[UpdateFlags_CF] ? flags_in[CF_IDX] : C;
        P <= update_flags[UpdateFlags_PF] ? flags_in[PF_IDX] : P;
        A <= update_flags[UpdateFlags_AF] ? flags_in[AF_IDX] : A;
        Z <= update_flags[UpdateFlags_ZF] ? flags_in[ZF_IDX] : Z;
        S <= update_flags[UpdateFlags_SF] ? flags_in[SF_IDX] : S;
        T <= update_flags[UpdateFlags_TF] ? flags_in[TF_IDX] : T;
        I <= update_flags[UpdateFlags_IF] ? flags_in[IF_IDX] : I;
        D <= update_flags[UpdateFlags_DF] ? flags_in[DF_IDX] : D;
        O <= update_flags[UpdateFlags_OF] ? flags_in[OF_IDX] : O;
    end
end

endmodule
