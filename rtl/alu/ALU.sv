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
module ALU(input logic [15:0] a,
           input logic [15:0] b,
           output logic [31:0] out,
           input logic is_8_bit,
           input logic [`MC_ALUOp_t_BITS-1:0] op,
           input logic [15:0] flags_in,
           output logic [15:0] flags_out,
           input logic multibit_shift,
           input logic [4:0] shift_count,
           output logic busy);

always_comb begin
    flags_out = flags_in;
    out = 32'b0;
    busy = 1'b0;

    case (op)
    ALUOp_SELA: out[15:0] = a;
    ALUOp_SELB: out[15:0] = b;
    ALUOp_ADD: do_add(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_ADC: do_adc(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_AND: do_and(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_XOR: do_xor(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_OR: do_or(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_BOUNDH: do_bound(a, b, flags_in, flags_out);
    ALUOp_BOUNDL: do_bound(b, a, flags_in, flags_out);
    ALUOp_SUB: do_sub(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SUBREV: do_sub(out[15:0], is_8_bit, b, a, flags_in, flags_out);
    ALUOp_SBB: do_sbb(out[15:0], is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SBBREV: do_sbb(out[15:0], is_8_bit, b, a, flags_in, flags_out);
    ALUOp_ENTER_FRAME_TEMP_ADDR: do_enter_frame_temp_addr(out[15:0], a, b[4:0]);
    ALUOp_GETFLAGS: out[15:0] = flags_in;
    ALUOp_SETFLAGSA: flags_out = a;
    ALUOp_SETFLAGSB: flags_out = b;
    ALUOp_CMC: flags_out[CF_IDX] = ~flags_in[CF_IDX];
    ALUOp_SHR: do_shr(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_SHL: do_shl(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_SAR: do_sar(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_ROR: do_ror(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_ROL: do_rol(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_RCL: do_rcl(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_RCR: do_rcr(out, is_8_bit, a, shift_count, flags_in, flags_out, busy,
                      multibit_shift);
    ALUOp_NOT: do_not(out[15:0], a, flags_in, flags_out);
    ALUOp_AAA: do_aaa(out[15:0], a, flags_in, flags_out);
    ALUOp_AAS: do_aas(out[15:0], a, flags_in, flags_out);
    ALUOp_DAA: do_daa(out[15:0], a, flags_in, flags_out);
    ALUOp_DAS: do_das(out[15:0], a, flags_in, flags_out);
    ALUOp_MUL: do_mul(out, is_8_bit, a, b, flags_in, flags_out, 1'b0);
    ALUOp_IMUL: do_mul(out, is_8_bit, a, b, flags_in, flags_out, 1'b1);
    ALUOp_EXTEND: do_extend(out[15:0], is_8_bit, a);
    ALUOp_DIV: ; // Handled by Divider, shares the enumeration.
    ALUOp_IDIV: ; // Handled by Divider, shares the enumeration.
    ALUOp_NEXT: begin
        if (flags_in[DF_IDX])
            do_sub(out[15:0], 1'b0, a, b, flags_in, flags_out);
        else
            do_add(out[15:0], 1'b0, a, b, flags_in, flags_out);
    end
    // verilator coverage_off
    default: begin
`ifdef verilator
        invalid_opcode_assertion: assert(0) begin
            $display("oops!");
        end
`endif // verilator
    end
    // verilator coverage_on
    endcase
end

endmodule
