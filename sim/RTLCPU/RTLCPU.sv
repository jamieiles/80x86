// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

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
              input logic debug_wr_en,
              // Simulation control
              input logic cache_enabled);

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

wire[19:1] c_addr;
wire[15:0] c_data_in;
wire[15:0] c_data_out;
wire c_access;
wire c_ack;
wire c_wr_en;
wire [1:0] c_bytesel;

wire d_ack = io_m_ack | data_m_ack;
wire [15:0] d_data_in = d_io ? io_m_data_in : data_m_data_in;

Cache #(.lines(`CACHE_SIZE / 16))
      Cache(.m_addr(q_m_addr),
            .m_data_in(q_m_data_in),
            .m_data_out(q_m_data_out),
            .m_access(q_m_access),
            .m_ack(q_m_ack),
            .m_wr_en(q_m_wr_en),
            .m_bytesel(q_m_bytesel),
            .enabled(cache_enabled),
            .*);

MemArbiter MemArbiter(.a_m_addr(instr_m_addr),
                      .a_m_data_in(instr_m_data_in),
                      .a_m_data_out(16'b0),
                      .a_m_access(instr_m_access),
                      .a_m_ack(instr_m_ack),
                      .a_m_wr_en(1'b0),
                      .a_m_bytesel(2'b11),
                      .b_m_addr(data_m_addr),
                      .b_m_data_in(data_m_data_in),
                      .b_m_data_out(data_m_data_out),
                      .b_m_access(data_m_access & ~d_io),
                      .b_m_ack(data_m_ack),
                      .b_m_wr_en(data_m_wr_en),
                      .b_m_bytesel(data_m_bytesel),
                      .q_m_addr(c_addr),
                      .q_m_data_in(c_data_in),
                      .q_m_data_out(c_data_out),
                      .q_m_access(c_access),
                      .q_m_ack(c_ack),
                      .q_m_wr_en(c_wr_en),
                      .q_m_bytesel(c_bytesel),
                      // verilator lint_off PINCONNECTEMPTY
                      .q_b(),
                      // verilator lint_on PINCONNECTEMPTY
                      .*);

Core    Core(.data_m_ack(d_ack),
             .data_m_data_in(d_data_in),
             .*);

endmodule
