module ALU(input logic [15:0] a,
           input logic [15:0] b,
           output logic [31:0] out,
           input logic is_8_bit,
           input logic [`MC_ALUOp_t_BITS-1:0] op,
           // verilator lint_off UNUSED
           // verilator lint_off UNDRIVEN
           input logic [15:0] flags_in,
           output logic [15:0] flags_out,
           // verilator lint_on UNUSED
           // verilator lint_on UNDRIVEN
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
