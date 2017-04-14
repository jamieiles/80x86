module UartPorts #(parameter clk_freq = 50000000)
                  (input logic clk,
                   input logic reset,
                   input logic cs,
                   input logic [15:0] data_m_data_in,
                   output logic [15:0] data_m_data_out,
                   input logic [1:0] data_m_bytesel,
                   input logic data_m_wr_en,
                   input logic data_m_access,
                   output logic data_m_ack,
                   input logic rx,
                   output logic tx);

wire access_status = cs & data_m_access & data_m_bytesel[1];
wire access_data = cs & data_m_access & data_m_bytesel[0];

reg rdy_clr;
wire tx_busy;
wire rdy;
wire [7:0] dout;
wire [7:0] din = data_m_data_in[7:0];
wire wr_en = access_data & data_m_wr_en & data_m_bytesel[0];

wire [7:0] status = {6'b0, tx_busy, rdy};

Uart #(.clk_freq(clk_freq))
     Uart(.*);

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk) begin
    data_m_data_out <= 16'b0;

    if (access_status && !data_m_wr_en)
        data_m_data_out[15:8] <= status;

    if (access_data && !data_m_wr_en)
        data_m_data_out[7:0] <= dout;
end

always_ff @(posedge clk)
    rdy_clr <= access_data & ~data_m_wr_en;

endmodule
