module Fifo(input logic clk,
            input logic reset,
            // Write port
            input logic wr_en,
            input logic [data_width-1:0] wr_data,
            // Read port
            input logic rd_en,
            output logic [data_width-1:0] rd_data,
            output logic empty,
            output logic nearly_full,
            output logic full);

parameter data_width = 32;
parameter depth = 6;
parameter full_threshold = 2; // Number of entries free to be not-full

localparam ptr_bits = $clog2(depth);

reg [data_width-1:0] mem[depth-1:0];
reg [ptr_bits-1:0] rd_ptr;
reg [ptr_bits-1:0] wr_ptr;
reg [ptr_bits:0] count;

assign empty = reset || count == 0;
assign full = count == depth && !reset;
assign nearly_full = !reset && count > depth - full_threshold;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        wr_ptr <= {ptr_bits{1'b0}};
    end else if (wr_en && !full) begin
        mem[wr_ptr] <= wr_data;
        wr_ptr <= wr_ptr + 1'b1;
        if (wr_ptr == depth[ptr_bits-1:0] - 1'b1)
            wr_ptr <= {ptr_bits{1'b0}};
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        rd_ptr <= {ptr_bits{1'b0}};
        rd_data <= {data_width{1'b0}};
    end else if (rd_en && !empty) begin
        rd_data <= mem[rd_ptr];
        rd_ptr <= rd_ptr + 1'b1;
        if (rd_ptr == depth[ptr_bits-1:0] - 1'b1)
            rd_ptr <= {ptr_bits{1'b0}};
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        count <= {ptr_bits + 1{1'b0}};
    else if (wr_en && !full && !rd_en)
        count <= count + 1'b1;
    else if (rd_en && !empty && !wr_en)
        count <= count - 1'b1;
end

endmodule
