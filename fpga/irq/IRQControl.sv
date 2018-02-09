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
