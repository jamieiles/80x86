module LoopCounter(input logic clk,
                   input logic [4:0] count_in,
                   input logic load,
                   input logic next,
                   output logic done);

reg [4:0] count;

assign done = ~|count;

always_ff @(posedge clk)
    if (load)
        count <= count_in;
    else if (next && |count)
        count <= count - 1'b1;

endmodule
