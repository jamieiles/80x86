module LEDSRegister(input logic clk,
                    input logic reset,
                    output logic [`CONFIG_NUM_LEDS-1:0] leds_val,
                    input logic cs,
                    input logic [15:0] data_m_data_in,
                    input logic data_m_access,
                    output logic data_m_ack,
                    input logic data_m_wr_en,
                    input logic [1:0] data_m_bytesel);

always_ff @(posedge clk or posedge reset)
    if (reset)
        leds_val <= `CONFIG_NUM_LEDS'b0;
    else begin
        if (`CONFIG_NUM_LEDS > 8 && cs && data_m_access && data_m_bytesel[1]
            && data_m_wr_en)
            leds_val[`CONFIG_NUM_LEDS-1:8] <= data_m_data_in[`CONFIG_NUM_LEDS-1:8];
        if (cs && data_m_access && data_m_bytesel[0] && data_m_wr_en)
            leds_val[7:0] <= data_m_data_in[7:0];
    end

always_ff @(posedge clk)
    data_m_ack <= cs & data_m_access;

endmodule
