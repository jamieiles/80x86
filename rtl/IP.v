module IP(input logic clk,
          input logic reset,
          input logic inc,
          input logic wr_en,
          input logic [15:0] wr_val,
          output logic [15:0] val);

reg [15:0] val;

always @(posedge clk or posedge reset)
    if (reset)
        val <= 16'b0;
    else if (wr_en)
        val <= wr_val;
    else if (inc)
        val <= val + 1'b1;

endmodule
