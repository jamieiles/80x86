module LoadStore(input logic clk,
                 input logic reset,
                 // Memory Address Register
                 input logic write_mar,
                 input [19:0] mar_in,
                 // Memory Data Register
                 output [15:0] mdr_out,
                 // Memory bus
                 output logic [19:1] m_addr,
                 input logic [15:0] m_data_in,
                 output logic m_access,
                 input logic m_ack,
                 // Control
                 input logic start,
                 output logic complete
                 );

// verilator lint_off UNUSED
reg [19:0] mar;
// verilator lint_on UNUSED
reg [15:0] mdr;

assign m_access = start & ~complete;
assign m_addr = mar[19:1];
assign mdr_out = mdr;

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        mar <= 20'b0;
    else if (write_mar)
        mar <= mar_in;
end

always_ff @(posedge clk)
    complete <= m_ack;

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        mdr <= 16'b0;
    else if (m_ack)
        mdr <= m_data_in;
end

endmodule
