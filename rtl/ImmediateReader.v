module ImmediateReader(input logic clk,
                       input logic reset,
                       // Control.
                       input logic start,
                       output logic complete,
                       input logic is_8bit,
                       // Result.
                       output logic [15:0] immediate,
                       // Fifo Read Port.
                       output logic fifo_rd_en,
                       input logic [7:0] fifo_rd_data,
                       input logic fifo_empty);

assign fifo_rd_en = ~fifo_empty & (start | (_fetching & (_bytes_read < _num_bytes)));

reg [15:0] _immediate_buf;

wire [1:0] _num_bytes = is_8bit ? 2'h1 : 2'h2;
reg [1:0] _bytes_read;

reg _popped;
reg _fetch_busy;
wire _fetching = _fetch_busy & ~complete;

assign complete = reset ? 1'b0 : _bytes_read == _num_bytes && _popped;

always_comb begin
    if (_bytes_read == 2'd1 && _popped)
        immediate = {{8{fifo_rd_data[7]}}, fifo_rd_data[7:0]};
    else if (_bytes_read == 2'd2 && _popped && !is_8bit)
        immediate = {fifo_rd_data, _immediate_buf[7:0]};
    else
        immediate = _immediate_buf;
end

always_ff @(posedge clk or posedge reset)
    if (reset || complete)
        _fetch_busy <= 1'b0;
    else if (start)
        _fetch_busy <= 1'b1;

always_ff @(posedge clk or posedge reset) begin
    if (reset || start)
        _bytes_read <= 2'b0;
    else if (fifo_rd_en && _bytes_read != _num_bytes)
        _bytes_read <= _bytes_read + 2'b1;
end

always_ff @(posedge clk or posedge reset)
    _popped <= reset ? 1'b0 : fifo_rd_en;

always_ff @(posedge clk or posedge reset) begin
    if (reset || start) begin
        _immediate_buf <= 16'b0;
    end

    if (_popped && _bytes_read == 2'b1)
        _immediate_buf[7:0] <= fifo_rd_data;
        if (is_8bit)
            _immediate_buf[15:8] <= {8{fifo_rd_data[7]}};
    else if (_popped && _bytes_read == 2'd2)
        _immediate_buf[15:8] <= fifo_rd_data;
end

endmodule
