module ALU(input logic [15:0] a,
           input logic [15:0] b,
           output logic [15:0] out,
           input logic [`MC_ALUOp_t_BITS-1:0] op,
           // verilator lint_off UNUSED
           // verilator lint_off UNDRIVEN
           input logic [15:0] flags_in,
           output logic [15:0] flags_out);
           // verilator lint_on UNUSED
           // verilator lint_on UNDRIVEN

always_comb begin
    case (op)
    ALUOp_SELA: out = a;
    ALUOp_SELB: out = b;
    default: begin
`ifdef verilator
        invalid_opcode_assertion: assert(0) begin
            $display("oops!");
        end
`endif // verilator
    end
    endcase
end

endmodule
