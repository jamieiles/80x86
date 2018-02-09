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
module UartPorts #(parameter clk_freq = 50000000)
                  (input logic clk,
                   input logic reset,
                   input logic cs,
                   input logic [15:0] data_m_data_in,
                   output logic [15:0] data_m_data_out,
                   input logic [1:0] data_m_bytesel,
                   input logic data_m_wr_en,
                   input logic data_m_access,
                   output logic data_m_ack,
                   input logic rx,
                   output logic tx);

wire access_status = cs & data_m_access & data_m_bytesel[1];
wire access_data = cs & data_m_access & data_m_bytesel[0];

reg rdy_clr;
wire tx_busy;
wire rdy;
wire [7:0] dout;
wire [7:0] din = data_m_data_in[7:0];
wire wr_en = access_data & data_m_wr_en & data_m_bytesel[0];

wire [7:0] status = {6'b0, tx_busy, rdy};

wire [15:0] data_val = {
    access_status & ~data_m_wr_en ? status : 8'b0,
    access_data & ~data_m_wr_en ? dout : 8'b0
};

Uart #(.clk_freq(clk_freq))
     Uart(.*);

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk)
    data_m_data_out <= data_val;

always_ff @(posedge clk)
    rdy_clr <= access_data && !data_m_wr_en;

endmodule
