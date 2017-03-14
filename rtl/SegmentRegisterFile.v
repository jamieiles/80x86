module SegmentRegisterFile(input logic clk,
                           input logic reset,
                           // Read port.
                           input logic [1:0] rd_sel,
                           output logic [15:0] rd_val,
                           // Write port.
                           input logic wr_en,
                           input logic [1:0] wr_sel,
                           input logic [15:0] wr_val,
                           // CS port.
                           output logic [15:0] cs);

reg [15:0] registers[4];

integer i;

wire rd_bypass = wr_en && wr_sel == rd_sel;

assign cs = registers[CS];

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        for (i = 0; i < 4; ++i)
            registers[i] <= 16'b0;
    else if (wr_en)
        registers[wr_sel] <= wr_val;
end

always_ff @(posedge clk)
    rd_val <= rd_bypass ? wr_val : registers[rd_sel];

`ifdef verilator
export "DPI-C" function get_sr;

function [15:0] get_sr;
    input int regnum;
    get_sr = registers[regnum];
endfunction

export "DPI-C" function set_sr;

function [15:0] set_sr;
    input int regnum;
    input int val;
    registers[regnum] = val[15:0];
    set_sr = 16'b0;
endfunction
`endif

endmodule
