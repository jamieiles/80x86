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

assign m_access = (start | fetch_second) & ~complete & ~m_ack;
assign m_addr = {segment, 3'b0} + {3'b0, mar[15:1]} + {18'b0, fetch_second};
assign mdr_out = mdr;

wire unaligned = mar[0];
reg fetching;
reg fetch_second;

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        fetching <= 1'b0;
    else if (m_ack)
        fetching <= 1'b0;
    else if (start)
        fetching <= 1'b1;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        mar <= 16'b0;
    else if (write_mar)
        mar <= mar_in;
end

always_ff @(posedge clk) begin
    if (unaligned && m_ack && fetch_second)
        complete <= 1'b1;
    else if (!unaligned && m_ack)
        complete <= 1'b1;
    else
        complete <= 1'b0;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset || (start && !fetching)) begin
        fetch_second <= 1'b0;
        mdr <= 16'b0;
    end else if (m_ack && !unaligned) begin
        fetch_second <= 1'b0;
        mdr <= m_data_in;
    end else if (m_ack && unaligned && !fetch_second) begin
        fetch_second <= 1'b1;
        mdr[7:0] <= unaligned ? m_data_in[15:8] : m_data_in[7:0];
    end else if (m_ack && unaligned && fetch_second) begin
        fetch_second <= 1'b0;
        mdr[15:8] <= unaligned ? m_data_in[7:0] : m_data_in[15:8];
    end
end

endmodule
