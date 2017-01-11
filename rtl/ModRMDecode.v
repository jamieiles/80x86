`include "RegisterEnum.v"

module ModRMDecode(input logic clk,
                   input logic reset,
                   // Control.
                   input logic start,
                   output logic complete,
                   // Results
                   output logic [15:0] effective_address,
                   output logic [2:0] regnum,
                   output logic rm_is_reg,
                   output logic [2:0] rm_regnum,
                   // Registers.
                   output logic [2:0] reg_sel[2],
                   input logic [15:0] regs[2],
                   // Fifo Read Port.
                   output logic fifo_rd_en,
                   input logic [7:0] fifo_rd_data,
                   input logic fifo_empty);

assign fifo_rd_en = ~fifo_empty & (start | (_fetching & (_bytes_read < _num_bytes)));

reg [7:0] _modrm;
reg [15:0] _immediate_buf;
wire [15:0] _immediate;

reg [1:0] _num_bytes;
reg [1:0] _bytes_read;

reg _popped;
reg _fetch_busy;
wire _fetching = _fetch_busy & ~complete;

wire [1:0] _mod = _bytes_read == 2'd1 && _popped ? fifo_rd_data[7:6] : _modrm[7:6];
wire [2:0] _reg = _bytes_read == 2'd1 && _popped ? fifo_rd_data[5:3] : _modrm[5:3];
wire [2:0] _rm  = _bytes_read == 2'd1 && _popped ? fifo_rd_data[2:0] : _modrm[2:0];

wire _has_address = _mod != 2'b11;

assign complete = reset ? 1'b0 : _bytes_read == _num_bytes && (!_has_address || _registers_fetched);

reg _registers_fetched;

always_ff @(posedge clk or posedge reset)
    if (reset || complete)
        _fetch_busy <= 1'b0;
    else if (start)
        _fetch_busy <= 1'b1;

always_ff @(posedge clk or posedge reset)
    if (reset || complete)
        _registers_fetched <= 1'b0;
    else if (_bytes_read == 2'd1)
        _registers_fetched <= 1'b1;

always_comb begin
    if (_bytes_read == 2'd2 && _popped)
        _immediate = {{8{fifo_rd_data[7]}}, fifo_rd_data[7:0]};
    else if (_bytes_read == 2'd3 && _popped && _num_bytes == 2'd3)
        _immediate = {fifo_rd_data, _immediate_buf[7:0]};
    else
        _immediate = _immediate_buf;
end

always_comb begin
    case (_mod)
    2'b00: _num_bytes = _rm == 3'b110 ? 2'd3 : 2'd1;
    2'b01: _num_bytes = 2'd2;
    2'b10: _num_bytes = 2'd3;
    2'b11: _num_bytes = 2'd1;
    endcase
end

always_comb begin
    case (_rm)
    3'b000, 3'b001, 3'b111: reg_sel[0] = BX;
    3'b010, 3'b011, 3'b110: reg_sel[0] = BP;
    3'b100: reg_sel[0] = SI;
    3'b101: reg_sel[0] = DI;
    endcase

    case (_rm)
    3'b000, 3'b010: reg_sel[1] = SI;
    3'b001, 3'b011: reg_sel[1] = DI;
    default: reg_sel[1] = 0;
    endcase
end

always_comb begin
    regnum = _reg;
    rm_regnum = _rm;
    rm_is_reg = 1'b0;

    case (_mod)
    2'b00: begin
        case (_rm)
        3'b000, 3'b001, 3'b010, 3'b011: effective_address = regs[0] + regs[1];
        3'b100, 3'b101, 3'b111: effective_address = regs[0];
        3'b110: effective_address = _immediate;
        endcase
    end
    2'b01, 2'b10: begin
        case (_rm)
        3'b000, 3'b001, 3'b010, 3'b011: effective_address = regs[0] + regs[1] + _immediate;
        3'b100, 3'b101, 3'b110, 3'b111: effective_address = regs[0] + _immediate;
        endcase
    end
    2'b11: begin
        rm_regnum = _rm;
        rm_is_reg = 1'b1;
    end
    endcase
end

always_ff @(posedge clk or posedge reset) begin
    if (reset || start)
        _bytes_read <= 2'd0;
    else if (fifo_rd_en && _bytes_read != _num_bytes)
        _bytes_read <= _bytes_read + 1'b1;
end

always_ff @(posedge clk or posedge reset)
    _popped <= reset ? 1'b0 : fifo_rd_en;

always_ff @(posedge clk or posedge reset) begin
    if (reset || start) begin
        _modrm <= 8'b0;
        _immediate_buf <= 16'b0;
    end

    if (_popped && _bytes_read == 2'd1)
        _modrm <= fifo_rd_data;
    else if (_popped && _bytes_read == 2'd2)
        _immediate_buf[7:0] <= fifo_rd_data;
        if (_num_bytes == 2'd2)
            _immediate_buf[15:8] <= {8{fifo_rd_data[7]}};
    else if (_popped && _bytes_read == 2'd3)
        _immediate_buf[15:8] <= fifo_rd_data;
end

endmodule
