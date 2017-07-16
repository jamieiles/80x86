module BIOSControlRegister(input logic clk,
                           input logic reset,
                           input logic cs,
                           output logic [15:0] data_m_data_out,
                           input logic [15:0] data_m_data_in,
                           input logic data_m_wr_en,
                           input logic data_m_access,
                           output logic data_m_ack,
                           output logic bios_enabled);

always_ff @(posedge clk) begin
    data_m_data_out <= data_m_access && cs && !data_m_wr_en ?
        {15'b0, bios_enabled} : 16'b0;
    data_m_ack <= data_m_access && cs;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        bios_enabled <= 1'b1;
    else if (cs && data_m_access && data_m_wr_en)
        bios_enabled <= data_m_data_in[0];
end

endmodule
