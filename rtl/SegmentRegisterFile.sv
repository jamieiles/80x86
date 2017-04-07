module SegmentRegisterFile(input logic clk,
                           input logic reset,
                           // Read port.
                           input logic [1:0] rd_sel,
                           output logic [15:0] rd_val,
                           // Write port.
                           input logic wr_en,
                           input logic [1:0] wr_sel,
                           input logic [15:0] wr_val,
                           // CS port.
                           output logic [15:0] cs);

reg [15:0] registers[4];

wire rd_bypass = wr_en && wr_sel == rd_sel;

assign cs = registers[CS];

always_ff @(posedge reset)
    ; // Reset is handled by the microcode

always_ff @(posedge clk) begin
    if (wr_en)
        registers[wr_sel] <= wr_val;
end

always_ff @(posedge clk)
    rd_val <= rd_bypass ? wr_val : registers[rd_sel];

endmodule
