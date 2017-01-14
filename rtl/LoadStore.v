module LoadStore(input logic clk,
                 input logic reset,
                 // Memory Address Register
                 input logic write_mar,
                 input logic [15:0] segment,
                 input [15:0] mar_in,
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
reg [15:0] mar;
// verilator lint_on UNUSED
reg [15:0] mdr;

assign m_access = (start | fetching | fetch_second) & ~complete & ~m_ack;
assign m_addr = {segment, 3'b0} + {3'b0, mar[15:1]};
assign mdr_out = mdr;

reg unaligned;
reg fetching;
reg fetch_second;

always_ff @(posedge clk or posedge reset)
    fetching <= start ? 1'b1 : reset || complete ? 1'b0 : fetching;

always_ff @(posedge clk or posedge reset)
    mar <= reset ? 16'b0 :
        m_ack ? mar + 16'b1 :
        write_mar ? mar_in : mar;

always_ff @(posedge clk)
    complete <= m_ack && ((unaligned && fetch_second) || !unaligned);

always_ff @(posedge clk or posedge reset)
    if (reset)
        unaligned <= 1'b0;
    else if (start && !fetching)
        unaligned <= mar[0];

always_ff @(posedge clk or posedge reset)
    if (m_ack && unaligned && !fetch_second)
        fetch_second <= 1'b1;
    else if (reset || (start && !fetching) || complete)
        fetch_second <= 1'b0;

always_ff @(posedge clk or posedge reset) begin
    if (reset || (start && !fetching)) begin
        mdr <= 16'b0;
    end else if (m_ack && !unaligned) begin
        mdr <= m_data_in;
    end else if (m_ack && unaligned && !fetch_second) begin
        mdr[7:0] <= unaligned ? m_data_in[15:8] : m_data_in[7:0];
    end else if (m_ack && unaligned && fetch_second) begin
        mdr[15:8] <= unaligned ? m_data_in[7:0] : m_data_in[15:8];
    end
end

endmodule
