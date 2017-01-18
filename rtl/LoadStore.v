module LoadStore(input logic clk,
                 input logic reset,
                 // Memory Address Register
                 input logic write_mar,
                 input logic [15:0] segment,
                 input [15:0] mar_in,
                 // Memory Data Register
                 output [15:0] mdr_out,
                 input logic write_mdr,
                 input [15:0] mdr_in,
                 // Memory bus
                 output logic [19:1] m_addr,
                 input logic [15:0] m_data_in,
                 output logic [15:0] m_data_out,
                 output logic m_access,
                 input logic m_ack,
                 output logic m_wr_en,
                 output logic [1:0] m_bytesel,
                 // Control
                 input logic start,
                 input logic is_8bit,
                 input logic wr_en,
                 output logic complete);

reg [15:0] mar;
reg [15:0] mdr;

assign m_access = (start | fetching | second_byte) & ~complete & ~m_ack;
assign m_addr = {segment, 3'b0} + {3'b0, mar[15:1]} + {18'b0, second_byte};
assign m_wr_en = wr_en;
assign mdr_out = mdr;

always_comb begin
    if (unaligned && !second_byte)
        m_data_out = {mdr[7:0], 8'b0};
    else if (unaligned && second_byte)
        m_data_out = {8'b0, mdr[15:8]};
    else
        m_data_out = mdr;
end

always_comb begin
    if (!is_8bit && !unaligned)
        m_bytesel = 2'b11;
    else if (unaligned && !second_byte)
        m_bytesel = 2'b10;
    else
        m_bytesel = 2'b01;
end

reg unaligned;
reg fetching;
reg second_byte;

always_ff @(posedge clk or posedge reset)
    fetching <= start ? 1'b1 : reset || complete ? 1'b0 : fetching;

always_ff @(posedge clk or posedge reset)
    mar <= reset ? 16'b0 :
        write_mar ? mar_in : mar;

always_ff @(posedge clk)
    complete <= m_ack && (is_8bit || (unaligned && second_byte) || !unaligned);

always_ff @(posedge clk or posedge reset)
    if (reset)
        unaligned <= 1'b0;
    else if (start && !fetching)
        unaligned <= mar[0];

always_ff @(posedge clk or posedge reset)
    if (m_ack && unaligned && !second_byte && !is_8bit)
        second_byte <= 1'b1;
    else if (reset || (start && !fetching) || complete)
        second_byte <= 1'b0;

always_ff @(posedge clk or posedge reset) begin
    if (reset || (start && !fetching && !wr_en))
        mdr <= 16'b0;
    else if (write_mdr)
        mdr <= mdr_in;
    else if (!wr_en && m_ack && !unaligned)
        mdr <= is_8bit ? {8'b0, m_data_in[7:0]} : m_data_in;
    else if (!wr_en && m_ack && unaligned && !second_byte)
        mdr[7:0] <= unaligned ? m_data_in[15:8] : m_data_in[7:0];
    else if (!wr_en && m_ack && unaligned && second_byte)
        mdr[15:8] <= unaligned ? m_data_in[7:0] : m_data_in[15:8];
end

endmodule
