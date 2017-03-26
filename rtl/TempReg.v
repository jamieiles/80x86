module TempReg(input logic clk,
               input logic reset,
               input logic [15:0] wr_val,
               input logic wr_en,
               output logic [15:0] val);

reg [15:0] r;

assign val = r;

always @(posedge clk or posedge reset)
    if (reset)
        r <= 16'b0;
    else if (wr_en)
        r <= wr_val;

endmodule
