module IRQController(input logic clk,
                     input logic reset,
                     input logic cs,
                     // verilator lint_off UNUSED
                     input logic [15:0] data_m_data_in,
                     input logic [1:0] data_m_bytesel,
                     // verilator lint_on UNUSED
                     output logic [15:0] data_m_data_out,
                     input logic data_m_wr_en,
                     input logic data_m_access,
                     output logic data_m_ack,
                     output logic nmi,
                     output logic [6:0] intr_test);

reg nmi_test;

wire access_test = cs & data_m_access & data_m_bytesel[0];

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk)
    data_m_data_out <= cs & data_m_access & ~data_m_wr_en ?
        {8'b0, nmi_test, intr_test} : 16'b0;

always_ff @(posedge clk or posedge reset)
    if (reset)
        {nmi_test, intr_test} <= 8'b0;
    else if (access_test & data_m_wr_en)
        {nmi_test, intr_test} <= data_m_data_in[7:0];

always_ff @(posedge clk or posedge reset)
    if (reset)
        nmi <= 1'b0;
    else
        nmi <= nmi_test;

endmodule
