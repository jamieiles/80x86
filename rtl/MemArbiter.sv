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
module MemArbiter(input logic clk,
                  input logic reset,
                  // Instruction bus
                  input logic [19:1] a_m_addr,
                  output logic [15:0] a_m_data_in,
                  input logic [15:0] a_m_data_out,
                  input logic a_m_access,
                  output logic a_m_ack,
                  input logic a_m_wr_en,
                  input logic [1:0] a_m_bytesel,
                  // Data bus
                  input logic [19:1] b_m_addr,
                  output logic [15:0] b_m_data_in,
                  input logic [15:0] b_m_data_out,
                  input logic b_m_access,
                  output logic b_m_ack,
                  input logic b_m_wr_en,
                  input logic [1:0] b_m_bytesel,
                  // Output bus
                  output logic [19:1] q_m_addr,
                  input logic [15:0] q_m_data_in,
                  output logic [15:0] q_m_data_out,
                  output logic q_m_access,
                  input logic q_m_ack,
                  output logic q_m_wr_en,
                  output logic [1:0] q_m_bytesel,
                  output logic q_b);

reg grant_to_b;
reg grant_active;

assign q_b = (grant_active && grant_to_b) || (!grant_active && b_m_access);

assign q_m_addr = q_b ? b_m_addr : a_m_addr;
assign q_m_data_out = q_b ? b_m_data_out : a_m_data_out;
assign q_m_access = ~q_m_ack & (b_m_access | a_m_access);
assign q_m_wr_en = q_b ? b_m_wr_en : a_m_wr_en;
assign q_m_bytesel = q_b ? b_m_bytesel : a_m_bytesel;

assign a_m_data_in = grant_active & ~grant_to_b ? q_m_data_in : 16'b0;
assign a_m_ack = grant_active & ~grant_to_b & q_m_ack;
assign b_m_data_in = grant_active & grant_to_b ? q_m_data_in : 16'b0;
assign b_m_ack = grant_active & grant_to_b & q_m_ack;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        grant_active <= 1'b0;
    end else begin
        if (q_m_ack)
            grant_active <= 1'b0;
        else if (!grant_active && (b_m_access || a_m_access)) begin
            grant_active <= 1'b1;
            grant_to_b <= b_m_access;
        end
    end
end

endmodule
