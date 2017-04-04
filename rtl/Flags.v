module Flags(input logic clk,
             input logic reset,
             // verilator lint_off UNUSED
             input logic [15:0] flags_in,
             // verilator lint_on UNUSED
             input logic [8:0] update_flags,
             output logic [15:0] flags_out);

localparam FLAGS_RESET = 16'h0002;

// verilator lint_off BLKANDNBLK
reg [15:0] flags;
// verilator lint_on BLKANDNBLK

assign flags_out = flags;

always_ff @(posedge clk or posedge reset) begin
    flags[CF_IDX] <=
        update_flags[UpdateFlags_CF] ? flags_in[CF_IDX] : flags[CF_IDX];
    flags[PF_IDX] <=
        update_flags[UpdateFlags_PF] ? flags_in[PF_IDX] : flags[PF_IDX];
    flags[AF_IDX] <=
        update_flags[UpdateFlags_AF] ? flags_in[AF_IDX] : flags[AF_IDX];
    flags[ZF_IDX] <=
        update_flags[UpdateFlags_ZF] ? flags_in[ZF_IDX] : flags[ZF_IDX];
    flags[SF_IDX] <=
        update_flags[UpdateFlags_SF] ? flags_in[SF_IDX] : flags[SF_IDX];
    flags[TF_IDX] <=
        update_flags[UpdateFlags_TF] ? flags_in[TF_IDX] : flags[TF_IDX];
    flags[IF_IDX] <=
        update_flags[UpdateFlags_IF] ? flags_in[IF_IDX] : flags[IF_IDX];
    flags[DF_IDX] <=
        update_flags[UpdateFlags_DF] ? flags_in[DF_IDX] : flags[DF_IDX];
    flags[OF_IDX] <=
        update_flags[UpdateFlags_OF] ? flags_in[OF_IDX] : flags[OF_IDX];
    if (reset)
        flags <= FLAGS_RESET;
end

endmodule
