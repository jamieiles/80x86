module ModRMDecode(input logic clk,
                   input logic reset,
                   // Control.
                   input logic start,
                   output logic busy,
                   output logic complete,
                   input logic clear,
                   // Results
                   output logic [15:0] effective_address,
                   output logic [2:0] regnum,
                   output logic rm_is_reg,
                   output logic [2:0] rm_regnum,
                   output logic bp_as_base,
                   // Registers.
                   output logic [2:0] reg_sel[2],
                   input logic [15:0] regs[2],
                   // Fifo Read Port.
                   output logic fifo_rd_en,
                   input logic [7:0] fifo_rd_data,
                   input logic fifo_empty,
                   // Immediates
                   output logic immed_start,
                   input logic immed_complete,
                   output logic immed_is_8bit,
                   input logic [15:0] immediate);

reg [7:0] _modrm;
reg _modrm_byte_read;
reg _started;
reg _registers_fetched;
reg [15:0] _cached_effective_address;
reg [15:0] _effective_address;
wire _has_address = _mod != 2'b11;
wire _fifo_valid = start & ~_modrm_byte_read & ~fifo_empty;
wire [1:0] _mod = _fifo_valid ? fifo_rd_data[7:6] : _modrm[7:6];
wire [2:0] _reg = _fifo_valid ? fifo_rd_data[5:3] : _modrm[5:3];
wire [2:0] _rm  = _fifo_valid ? fifo_rd_data[2:0] : _modrm[2:0];
wire _has_immediate = (_rm == 3'b110 && _mod == 2'b00) || ^_mod;

assign busy = (start | _started) & ~complete;
assign complete = reset ? 1'b0 :
    (_modrm_byte_read || _fifo_valid) &&
    (!_has_address || _registers_fetched) &&
    (!_has_immediate || immed_complete);
assign immed_start = _has_immediate && _modrm_byte_read;
assign immed_is_8bit = _mod == 2'b01;
assign effective_address = complete ? _effective_address : _cached_effective_address;

always_comb begin
    case (_mod)
    2'b00: bp_as_base = (_rm == 3'b010 || _rm == 3'b011);
    2'b01: bp_as_base = (_rm == 3'b010 || _rm == 3'b011 || _rm == 3'b110);
    2'b10: bp_as_base = (_rm == 3'b010 || _rm == 3'b011 || _rm == 3'b110);
    default: bp_as_base = 1'b0;
    endcase
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        _started <= 1'b0;
    else if (complete)
        _started <= 1'b0;
    else if (start)
        _started <= 1'b1;
end

always_ff @(posedge clk or posedge reset)
    if (reset)
        _registers_fetched <= 1'b0;
    else if (complete)
        _registers_fetched <= 1'b0;
    else if (~fifo_empty & start & ~_modrm_byte_read)
        _registers_fetched <= 1'b1;

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
    _effective_address = 16'b0;

    case (_mod)
    2'b00: begin
        case (_rm)
        3'b000, 3'b001, 3'b010, 3'b011: _effective_address = regs[0] + regs[1];
        3'b100, 3'b101, 3'b111: _effective_address = regs[0];
        3'b110: _effective_address = immediate;
        endcase
    end
    2'b01, 2'b10: begin
        case (_rm)
        3'b000, 3'b001, 3'b010, 3'b011: _effective_address = regs[0] + regs[1] + immediate;
        3'b100, 3'b101, 3'b110, 3'b111: _effective_address = regs[0] + immediate;
        endcase
    end
    2'b11: begin
        rm_regnum = _rm;
        rm_is_reg = 1'b1;
    end
    endcase
end

always_ff @(posedge clk)
    if (complete)
        _cached_effective_address <= _effective_address;

always_ff @(posedge clk or posedge reset)
    if (reset)
        _modrm_byte_read <= 1'b0;
    else if (complete)
        _modrm_byte_read <= 1'b0;
    else if (fifo_rd_en)
        _modrm_byte_read <= 1;

assign fifo_rd_en = ~fifo_empty & start & ~_modrm_byte_read;

always_ff @(posedge clk or posedge reset)
    if (reset)
        _modrm <= 8'b0;
    else if (clear)
        _modrm <= 8'b0;
    else if (~fifo_empty & start & ~_modrm_byte_read)
        _modrm <= fifo_rd_data;

endmodule
