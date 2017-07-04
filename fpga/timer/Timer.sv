module Timer(input logic clk,
             input logic reset,
             input logic cs,
             input logic [1:1] data_m_addr,
             input logic [15:0] data_m_data_in,
             output logic [15:0] data_m_data_out,
             input logic [1:0] data_m_bytesel,
             input logic data_m_wr_en,
             input logic data_m_access,
             output logic data_m_ack,
             output logic intr);

parameter clkf = 50000000;

localparam ticks_per_ms = clkf / 1000;

localparam ms_tick_bits = $clog2(ticks_per_ms);
reg [ms_tick_bits-1:0] ms_counter;

reg [14:0] counter;
reg [14:0] reload;
reg enabled;

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk)
    data_m_data_out <= cs & data_m_access & ~data_m_wr_en ?
        {enabled, counter} : 16'b0;

always_ff @(posedge clk or posedge reset)
    if (reset) begin
        counter <= 15'b0;
        ms_counter <= ms_tick_bits'(1'b0);
        enabled <= 1'b0;
        reload <= 15'b0;
    end else if (cs & data_m_access & data_m_wr_en) begin
        {enabled, counter} <= data_m_data_in;
        ms_counter <= ms_tick_bits'(ticks_per_ms);
        reload <= data_m_data_in[14:0];
    end else if (enabled) begin
        ms_counter <= |ms_counter ? ms_counter - 1'b1 :
            ms_tick_bits'(ticks_per_ms);

        if (~|ms_counter)
            counter <= |counter ? counter - 1'b1 : reload;
    end

always_ff @(posedge clk or posedge reset)
    if (reset)
        intr <= 1'b0;
    else if (cs && data_m_access)
        intr <= 1'b0;
    else if (enabled & |reload & ~|counter & ~|ms_counter)
        intr <= 1'b1;

endmodule
