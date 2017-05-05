module PosedgeToPulse(input logic clk,
                      input logic reset,
                      input logic d,
                      output logic q);

reg d_prev;

always_ff @(posedge clk)
    d_prev <= d;

always_ff @(posedge clk or posedge reset)
    if (reset) begin
        q <= 1'b0;
    end else begin
        q <= (d ^ d_prev) & d;
    end

endmodule
