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

task do_add;
input [15:0] _a;
input [15:0] _b;
input _carry_in;
begin
    if (!is_8_bit) begin
        {flags_out[CF_IDX], out} = _a + _b + {15'b0, _carry_in};
        flags_out[OF_IDX] = ~(_a[15] ^ _b[15]) & (out[15] ^ _b[15]);
    end else begin
        out = a + b + {15'b0, _carry_in};
        flags_out[CF_IDX] = _a[8] ^ _b[8] ^ out[8];
        flags_out[OF_IDX] = ~(_a[7] ^ _b[7]) & (out[7] ^ _b[7]);
    end
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[AF_IDX] = a[4] ^ b[4] ^ out[4];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;
end
endtask

task do_sub;
input [15:0] _a;
input [15:0] _b;
input _carry_in;
begin
    if (!is_8_bit) begin
        {flags_out[CF_IDX], out} = _a - _b - {15'b0, _carry_in};
        flags_out[OF_IDX] = (_a[15] ^ _b[15]) & (out[15] ^ _a[15]);
    end else begin
        out = _a - _b - {15'b0, _carry_in};
        flags_out[CF_IDX] = _a[8] ^ _b[8] ^ out[8];
        flags_out[OF_IDX] = (_a[7] ^ _b[7]) & (out[7] ^ _a[7]);
    end
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[AF_IDX] = a[4] ^ b[4] ^ out[4];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;
end
endtask

task do_and;
input [15:0] _a;
input [15:0] _b;
begin
    out = _a & _b;
    {flags_out[CF_IDX], flags_out[OF_IDX]} = 2'b0;
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[AF_IDX] = a[4] ^ b[4] ^ out[4];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;
end
endtask

task do_xor;
input [15:0] _a;
input [15:0] _b;
begin
    out = _a ^ _b;
    {flags_out[CF_IDX], flags_out[OF_IDX]} = 2'b0;
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[AF_IDX] = a[4] ^ b[4] ^ out[4];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;
end
endtask

task do_or;
input [15:0] _a;
input [15:0] _b;
begin
    out = _a | _b;
    {flags_out[CF_IDX], flags_out[OF_IDX]} = 2'b0;
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[AF_IDX] = a[4] ^ b[4] ^ out[4];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;
end
endtask

task do_shr;
input [15:0] _a;
input [4:0] _b;
begin
    {out, flags_out[CF_IDX]} = {_a, 1'b0} >> _b[4:0];
    flags_out[OF_IDX] = _a[is_8_bit ? 7 : 15];
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;

    if (~|_b)
        flags_out = flags_in;
end
endtask

task do_shl;
input [15:0] _a;
input [4:0] _b;
begin
    if (!is_8_bit) begin
        {flags_out[CF_IDX], out} = {1'b0, _a} << _b[4:0];
        flags_out[OF_IDX] = _a[15] ^ out[15];
    end else begin
        {flags_out[CF_IDX], out[7:0]} = {1'b0, _a[7:0]} << _b[4:0];
        flags_out[OF_IDX] = _a[7] ^ out[7];
    end
    flags_out[PF_IDX] = ~^out[7:0];
    flags_out[SF_IDX] = out[is_8_bit ? 7 : 15];
    flags_out[ZF_IDX] = is_8_bit ? ~|out[7:0] : ~|out;

    if (~|_b)
        flags_out = flags_in;
end
endtask

always_comb begin
    case (op)
    ALUOp_SELA: out = a;
    ALUOp_SELB: out = b;
    ALUOp_ADD: do_add(a, b, 1'b0);
    ALUOp_ADC: do_add(a, b, flags_in[CF_IDX]);
    ALUOp_AND: do_and(a, b);
    ALUOp_XOR: do_xor(a, b);
    ALUOp_OR: do_or(a, b);
    ALUOp_SUB: do_sub(a, b, 1'b0);
    ALUOp_SUBREV: do_sub(b, a, 1'b0);
    ALUOp_SBB: do_sub(a, b, flags_in[CF_IDX]);
    ALUOp_SBBREV: do_sub(b, a, flags_in[CF_IDX]);
    ALUOp_GETFLAGS: out = flags_in;
    ALUOp_SETFLAGSA: flags_out = a;
    ALUOp_SETFLAGSB: flags_out = b;
    ALUOp_CMC: flags_out[CF_IDX] = ~flags_in[CF_IDX];
    ALUOp_SHR: do_shr(a, b[4:0]);
    ALUOp_SHL: do_shl(a, b[4:0]);
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
