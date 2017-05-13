module ImmediateReader(input logic clk,
                       input logic reset,
                       // Control.
                       input logic start,
                       output logic busy,
                       output logic complete,
                       input logic is_8bit,
                       // Result.
                       output logic [15:0] immediate,
                       // Fifo Read Port.
                       output logic fifo_rd_en,
                       input logic [7:0] fifo_rd_data,
                       input logic fifo_empty);

assign fifo_rd_en = ~fifo_empty & (start | (_fetching & ~complete));

reg [15:0] _immediate_buf;

wire [1:0] _num_bytes = is_8bit ? 2'h1 : 2'h2;
reg [1:0] _bytes_read;

reg _fetch_busy;
wire _fetching = _fetch_busy & ~complete;

assign complete = reset ? 1'b0 : _bytes_read == _num_bytes - 1'b1 &&
    ~fifo_empty && (start || _fetch_busy);

assign busy = (start | _fetch_busy) & ~complete;

always_comb begin
    if (_bytes_read == 2'd0 && ~fifo_empty && start)
        immediate = {{8{fifo_rd_data[7]}}, fifo_rd_data[7:0]};
    else if (_bytes_read == 2'd1 && ~fifo_empty && !is_8bit)
        immediate = {fifo_rd_data, _immediate_buf[7:0]};
    else
        immediate = _immediate_buf;
end

always_ff @(posedge clk or posedge reset)
    if (reset)
        _fetch_busy <= 1'b0;
    else if (complete)
        _fetch_busy <= 1'b0;
    else if (start)
        _fetch_busy <= 1'b1;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        _bytes_read <= 2'b0;
    end else begin
        if ((start && !_fetch_busy) || complete)
            _bytes_read <= 2'b0;
        if (fifo_rd_en && !complete)
            _bytes_read <= _bytes_read + 2'b1;
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        _immediate_buf <= 16'b0;
    end else begin
        if (start && !_fetch_busy) begin
            _immediate_buf <= 16'b0;
        end

        if (_bytes_read == 2'b0 && ~fifo_empty && start) begin
            _immediate_buf[7:0] <= fifo_rd_data;
            if (is_8bit)
                _immediate_buf[15:8] <= {8{fifo_rd_data[7]}};
        end else if (_bytes_read == 2'd1 && ~fifo_empty)
            _immediate_buf[15:8] <= fifo_rd_data;
    end
end

endmodule
