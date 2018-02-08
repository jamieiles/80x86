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

reg grant_to_data;
reg grant_active;

wire q_data = (grant_active && grant_to_data) || (!grant_active && data_m_access);

assign q_m_addr = q_data ? data_m_addr : instr_m_addr;
assign q_m_data_out = data_m_data_out;
assign q_m_access = ~q_m_ack & (data_m_access | instr_m_access);
assign q_m_wr_en = q_data ? data_m_wr_en : 1'b0;
assign q_m_bytesel = q_data ? data_m_bytesel : 2'b11;

assign instr_m_data_in = q_m_data_in;
assign instr_m_ack = grant_active & ~grant_to_data & q_m_ack;
assign data_m_data_in = q_m_data_in;
assign data_m_ack = grant_active & grant_to_data & q_m_ack;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        grant_active <= 1'b0;
    end else begin
        if (q_m_ack)
            grant_active <= 1'b0;
        else if (!grant_active && (data_m_access || instr_m_access)) begin
            grant_active <= 1'b1;
            grant_to_data <= data_m_access;
        end
    end
end

endmodule
