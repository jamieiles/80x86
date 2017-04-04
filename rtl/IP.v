module IP(input logic clk,
          input logic reset,
          input logic inc,
          input logic wr_en,
          input logic [15:0] wr_val,
          output logic [15:0] val);

// verilator lint_off BLKANDNBLK
reg [15:0] cur_val;
// verilator lint_on BLKANDNBLK
assign val = cur_val;

always @(posedge clk or posedge reset)
    if (reset)
        cur_val <= 16'b0;
    else if (wr_en)
        cur_val <= wr_val;
    else if (inc)
        cur_val <= cur_val + 1'b1;

endmodule
