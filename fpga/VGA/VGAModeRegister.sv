module VGAModeRegister(input logic clk,
                       input logic reset,
                       input logic cs,
                       output logic [15:0] data_m_data_out,
                       input logic data_m_wr_en,
                       input logic data_m_access,
                       output logic data_m_ack);

always_ff @(posedge clk) begin
    data_m_data_out <= data_m_access && cs && !data_m_wr_en ?
        {16'd9} : 16'b0;
    data_m_ack <= data_m_access && cs;
end

endmodule
