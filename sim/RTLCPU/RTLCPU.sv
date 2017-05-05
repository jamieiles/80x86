module RTLCPU(input logic clk,
              input logic reset,
              // Interrupts
              input logic nmi,
              input logic intr,
              input logic [7:0] irq,
              output logic inta,
              // Memory bus
              output logic [19:1] q_m_addr,
              input logic [15:0] q_m_data_in,
              output logic [15:0] q_m_data_out,
              output logic q_m_access,
              input logic q_m_ack,
              output logic q_m_wr_en,
              output logic [1:0] q_m_bytesel,
              // IO bus
              output logic [15:1] io_m_addr,
              input logic [15:0] io_m_data_in,
              output logic [15:0] io_m_data_out,
              output logic io_m_access,
              input logic io_m_ack,
              output logic io_m_wr_en,
              output logic [1:0] io_m_bytesel,
              // Misc
              output logic d_io,
              output logic lock,
              // Debug
              output logic debug_stopped,
              input logic debug_seize,
              input logic [7:0] debug_addr,
              input logic debug_run,
              output logic [15:0] debug_val,
              input logic [15:0] debug_wr_val,
              input logic debug_wr_en);

// Instruction bus
logic [19:1] instr_m_addr;
logic [15:0] instr_m_data_in;
logic instr_m_access;
logic instr_m_ack;
// Data bus
logic [19:1] data_m_addr;
logic [15:0] data_m_data_in;
logic [15:0] data_m_data_out;
logic data_m_access;
logic data_m_ack;
logic data_m_wr_en;
logic [1:0] data_m_bytesel;

assign io_m_addr = data_m_addr[15:1];
assign io_m_data_out = data_m_data_out;
assign io_m_access = data_m_access & d_io;
assign io_m_wr_en = data_m_wr_en;
assign io_m_bytesel = data_m_bytesel;

wire d_ack = io_m_ack | data_m_ack;
wire [15:0] d_data_in = d_io ? io_m_data_in : data_m_data_in;

MemArbiter MemArbiter(.data_m_access(data_m_access & ~d_io),
                      .*);
Core    Core(.data_m_ack(d_ack),
             .data_m_data_in(d_data_in),
             .*);

endmodule
