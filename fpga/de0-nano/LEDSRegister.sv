module LEDSRegister(input logic clk,
                    input logic reset,
                    output logic [7:0] leds_val,
                    input logic cs,
                    input logic [15:0] data_m_data_in,
                    input logic data_m_access,
                    output logic data_m_ack,
                    input logic data_m_wr_en,
                    input logic [1:0] data_m_bytesel);

always_ff @(posedge clk or posedge reset)
    if (reset)
        leds_val <= 8'b0;
    else if (cs && data_m_access && data_m_bytesel[0] && data_m_wr_en)
        leds_val <= data_m_data_in[7:0];

always_ff @(posedge clk or posedge reset)
    data_m_ack <= cs & data_m_access;

endmodule
