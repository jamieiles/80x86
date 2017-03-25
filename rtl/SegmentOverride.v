module SegmentOverride(input logic clk,
                       input logic reset,
                       input logic next_instruction,
                       input logic force_segment,
                       input logic bp_is_base,
                       input logic segment_override,
                       input logic [1:0] microcode_sr_rd_sel,
                       output logic [1:0] sr_rd_sel);

reg [1:0] override;
reg override_active;

always_comb begin
    if (force_segment || segment_override)
        sr_rd_sel = microcode_sr_rd_sel;
    else if (override_active)
        sr_rd_sel = override;
    else if (bp_is_base)
        sr_rd_sel = SS;
    else
        sr_rd_sel = microcode_sr_rd_sel;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset || next_instruction) begin
        override_active <= 1'b0;
    end else if (segment_override) begin
        override <= microcode_sr_rd_sel;
        override_active <= 1'b1;
    end
end

endmodule
