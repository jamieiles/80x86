module BitSync(input logic clk,
               input logic d,
               output logic q);

reg p1;
reg p2;

assign q = p2;

always_ff @(posedge clk)
    p1 <= d;
always_ff @(posedge clk)
    p2 <= p1;

endmodule
