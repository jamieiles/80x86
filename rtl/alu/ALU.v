module ALU(input logic [15:0] a,
           input logic [15:0] b,
           output logic [15:0] out,
           input logic is_8_bit,
           input logic [`MC_ALUOp_t_BITS-1:0] op,
           // verilator lint_off UNUSED
           // verilator lint_off UNDRIVEN
           input logic [15:0] flags_in,
           output logic [15:0] flags_out);
           // verilator lint_on UNUSED
           // verilator lint_on UNDRIVEN

always_comb begin
    flags_out = flags_in;
    case (op)
    ALUOp_SELA: out = a;
    ALUOp_SELB: out = b;
    ALUOp_ADD: do_add(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_ADC: do_adc(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_AND: do_and(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_XOR: do_xor(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_OR: do_or(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SUB: do_sub(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SUBREV: do_sub(out, is_8_bit, b, a, flags_in, flags_out);
    ALUOp_SBB: do_sbb(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SBBREV: do_sbb(out, is_8_bit, b, a, flags_in, flags_out);
    ALUOp_GETFLAGS: out = flags_in;
    ALUOp_SETFLAGSA: flags_out = a;
    ALUOp_SETFLAGSB: flags_out = b;
    ALUOp_CMC: flags_out[CF_IDX] = ~flags_in[CF_IDX];
    ALUOp_SHR: do_shr(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SHL: do_shl(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_SAR: do_sar(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_ROR: do_ror(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_ROL: do_rol(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_RCL: do_rcl(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_RCR: do_rcr(out, is_8_bit, a, b, flags_in, flags_out);
    ALUOp_NOT: do_not(out, a, flags_in, flags_out);
    ALUOp_NEXT: begin
        if (flags_in[DF_IDX])
            do_sub(out, 1'b0, a, b, flags_in, flags_out);
        else
            do_add(out, 1'b0, a, b, flags_in, flags_out);
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
