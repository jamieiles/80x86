module Fifo(input logic clk,
            input logic reset,
            /* Write port */
            input logic wr_en,
            input logic [31:0] wr_data,
            /* Read port */
            input logic rd_en,
            output logic [31:0] rd_data,
            output logic empty,
            output logic full);

reg [31:0] val = 32'b0;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        empty <= 1'b1;
        full <= 1'b0;
    end else begin
        if (wr_en) begin
            empty <= 1'b0;
            val <= wr_data;
        end
        if (rd_en) begin
            empty <= 1'b1;
            rd_data <= val;
        end
    end
end

endmodule
