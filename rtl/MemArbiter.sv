module MemArbiter(input logic clk,
                  input logic reset,
                  // Instruction bus
                  input logic [19:1] instr_m_addr,
                  output logic [15:0] instr_m_data_in,
                  input logic instr_m_access,
                  output logic instr_m_ack,
                  // Data bus
                  input logic [19:1] data_m_addr,
                  output logic [15:0] data_m_data_in,
                  input logic [15:0] data_m_data_out,
                  input logic data_m_access,
                  output logic data_m_ack,
                  input logic data_m_wr_en,
                  input logic [1:0] data_m_bytesel,
                  // Output bus
                  output logic [19:1] q_m_addr,
                  input logic [15:0] q_m_data_in,
                  output logic [15:0] q_m_data_out,
                  output logic q_m_access,
                  input logic q_m_ack,
                  output logic q_m_wr_en,
                  output logic [1:0] q_m_bytesel);

reg instr_grant;
reg data_grant;

wire q_data = data_will_grant | (data_grant & ~data_m_ack);

assign q_m_addr = q_data ? data_m_addr : instr_m_addr;
assign q_m_data_out = data_m_data_out;
assign q_m_access = q_data ? data_m_access : instr_m_access;
assign q_m_wr_en = q_data ? data_m_wr_en : 1'b0;
assign q_m_bytesel = q_data ? data_m_bytesel : 2'b11;

assign instr_m_data_in = q_m_data_in;
assign instr_m_ack = instr_grant & q_m_ack;
assign data_m_data_in = q_m_data_in;
assign data_m_ack = data_grant & q_m_ack;

// Data takes priority over instruction accesses to complete instructions.
wire instr_will_grant = instr_m_access & ~data_m_access & ~data_grant;
wire data_will_grant = data_m_access & ~instr_grant;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        instr_grant <= 1'b0;
        data_grant <= 1'b0;
    end else begin
        if (data_m_ack)
            data_grant <= 1'b0;
        if (instr_m_ack)
            instr_grant <= 1'b0;

        if (data_will_grant)
            data_grant <= 1'b1;
        else if (instr_will_grant)
            instr_grant <= 1'b1;
    end
end

endmodule
