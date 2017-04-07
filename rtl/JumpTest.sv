module JumpTest(input logic [7:0] opcode,
                // verilator lint_off UNUSED
                input logic [15:0] flags,
                // verilator lint_on UNUSED
                output logic taken);

always_comb begin
    taken = 1'b0;

    unique casez(opcode)
    8'b011?0000: taken = flags[OF_IDX]; // JO
    8'b011?0001: taken = ~flags[OF_IDX]; // JNO
    8'b011?0010: taken = flags[CF_IDX]; // JB
    8'b011?0011: taken = ~flags[CF_IDX]; // JNB
    8'b011?0100: taken = flags[ZF_IDX]; // JE
    8'b011?0101: taken = ~flags[ZF_IDX]; // JNE
    8'b011?0110: taken = flags[CF_IDX] | flags[ZF_IDX]; // JBE
    8'b011?0111: taken = ~(flags[CF_IDX] | flags[ZF_IDX]); // JNBE
    8'b011?1000: taken = flags[SF_IDX]; // JS
    8'b011?1001: taken = ~flags[SF_IDX]; // JNS
    8'b011?1010: taken = flags[PF_IDX]; // JP
    8'b011?1011: taken = ~flags[PF_IDX]; // JNP
    8'b011?1100: taken = flags[OF_IDX] ^ flags[SF_IDX]; // JL
    8'b011?1101: taken = ~(flags[SF_IDX] ^ flags[OF_IDX]); // JNL
    8'b011?1110: taken = (flags[OF_IDX] ^ flags[SF_IDX]) | flags[ZF_IDX]; // JLE
    8'b011?1111: taken = ~((flags[SF_IDX] ^ flags[OF_IDX]) | flags[ZF_IDX]); // JNLE
    8'b11001110: taken = flags[OF_IDX]; // INTO
    default: taken = 1'b0;
    endcase
end

endmodule
