module IRQController(input logic clk,
                     input logic reset,
                     input logic cs,
                     input logic [1:1] data_m_addr,
                     input logic [15:0] data_m_data_in,
                     output logic [15:0] data_m_data_out,
                     input logic [1:0] data_m_bytesel,
                     input logic data_m_wr_en,
                     input logic data_m_access,
                     output logic data_m_ack,
                     output logic nmi,
                     output logic intr,
                     output logic [7:0] irq,
                     input logic [7:0] intr_in);

reg [7:0] enable_reg;
reg [7:0] test_reg;
reg [7:0] base_reg;

wire access_test = cs & data_m_access & data_m_addr[1] & data_m_bytesel[0];
wire access_enable = cs & data_m_access & ~data_m_addr[1] & data_m_bytesel[0];
wire access_base = cs & data_m_access & ~data_m_addr[1] & data_m_bytesel[1];

wire [7:0] data_out_low = access_enable ? enable_reg :
                          access_test ? test_reg : 8'b0;
wire [7:0] data_out_high = access_base ? base_reg : 8'b0;

assign intr = |irq;

wire [7:0] pending = test_reg | (enable_reg & intr_in);

int i;
always_ff @(posedge clk or posedge reset)
    if (reset)
        irq <= 8'b0;
    else if (~|pending)
        irq <= 8'b0;
    else if (|pending) begin
        for (i = 0; i < 7; i += 1) begin
            if (pending[i]) begin
                irq <= i[7:0] + base_reg;
                break;
            end
        end
    end

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk)
    data_m_data_out <= cs & data_m_access & ~data_m_wr_en ?
        {data_out_high, data_out_low} : 16'b0;

always_ff @(posedge clk or posedge reset)
    if (reset)
        test_reg <= 8'b0;
    else if (access_test & data_m_wr_en)
        test_reg <= data_m_data_in[7:0];

always_ff @(posedge clk or posedge reset)
    if (reset)
        base_reg <= 8'b0;
    else if (access_base & data_m_wr_en)
        base_reg <= data_m_data_in[15:8];

always_ff @(posedge clk or posedge reset)
    if (reset)
        enable_reg <= 8'b0;
    else if (access_enable & data_m_wr_en)
        enable_reg <= data_m_data_in[7:0];

always_ff @(posedge clk or posedge reset)
    if (reset)
        nmi <= 1'b0;
    else
        nmi <= test_reg[7];

endmodule
