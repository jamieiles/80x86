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

`default_nettype none
module Timer(input logic clk,
             input logic reset,
             input logic pit_clk,
             input logic cs,
             input logic [1:1] data_m_addr,
             input logic [15:0] data_m_data_in,
             output logic [15:0] data_m_data_out,
             input logic [1:0] data_m_bytesel,
             input logic data_m_wr_en,
             input logic data_m_access,
             output logic data_m_ack,
             output logic intr,
             output logic speaker_out,
             input logic speaker_gate_en);

wire pit_clk_sync;
wire access_timer0 = cs & data_m_access & ~data_m_addr[1] & data_m_bytesel[0];
wire access_timer2 = cs & data_m_access & data_m_addr[1] & data_m_bytesel[0];
wire access_ctrl = cs & data_m_access & data_m_addr[1] & data_m_bytesel[1];
// verilator lint_off UNUSED
wire [7:0] ctrl_wr_val = data_m_data_in[15:8];
// verilator lint_on UNUSED
wire [1:0] channel = ctrl_wr_val[7:6];

wire [7:0] timer0_count;
wire [7:0] timer2_count;

BitSync PITSync(.clk(clk),
                .reset(reset),
                .d(pit_clk),
                .q(pit_clk_sync));

TimerUnit Timer0(.pit_clk(pit_clk_sync),
                 .reload_in(data_m_data_in[7:0]),
                 .load(access_timer0 & data_m_wr_en),
                 .rw_in(ctrl_wr_val[5:4]),
                 .mode_in(ctrl_wr_val[2:1]),
                 .configure(access_ctrl && data_m_wr_en && channel == 2'b00 && ctrl_wr_val[5:4] != 2'b00),
                 .latch_count(access_ctrl && data_m_wr_en && channel == 2'b00 && ctrl_wr_val[5:4] == 2'b00),
                 .read_count(access_timer0 && !data_m_wr_en),
                 .count_out(timer0_count),
                 .out(intr),
                 .gate(1'b1),
                 .*);

TimerUnit Timer2(.pit_clk(pit_clk_sync),
                 .reload_in(data_m_data_in[7:0]),
                 .load(access_timer2 & data_m_wr_en),
                 .rw_in(ctrl_wr_val[5:4]),
                 .mode_in(ctrl_wr_val[2:1]),
                 .configure(access_ctrl && data_m_wr_en && channel == 2'b10 && ctrl_wr_val[5:4] != 2'b00),
                 .latch_count(access_ctrl && data_m_wr_en && channel == 2'b10 && ctrl_wr_val[5:4] == 2'b00),
                 .read_count(access_timer2 && !data_m_wr_en),
                 .count_out(timer2_count),
                 .out(speaker_out),
                 .gate(speaker_gate_en),
                 .*);

always_ff @(posedge clk)
    if (access_timer0 && !data_m_wr_en)
        data_m_data_out <= {8'b0, timer0_count};
    else if (access_timer2 && !data_m_wr_en)
        data_m_data_out <= {8'b0, timer2_count};
    else
        data_m_data_out <= 16'b0;

always_ff @(posedge clk)
    data_m_ack <= cs & data_m_access;

endmodule
