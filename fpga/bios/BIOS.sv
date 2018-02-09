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
module BIOS #(parameter depth = 32)
             (input logic clk,
              input logic cs,
              input logic data_m_access,
              output logic data_m_ack,
              input logic [19:1] data_m_addr,
              input logic data_m_wr_en,
              input logic [15:0] data_m_data_in,
              output logic [15:0] data_m_data_out,
              input logic [1:0] data_m_bytesel);

wire wr_en = data_m_access & cs & data_m_wr_en;
wire [15:0] q;
assign data_m_data_out = data_m_ack ? q : 16'b0;

always_ff @(posedge clk)
    data_m_ack <= cs & data_m_access;

altsyncram	altsyncram_component(.address_a(data_m_addr[$clog2(depth):1]),
                                     .byteena_a(data_m_bytesel),
                                     .clock0(clk),
                                     .data_a(data_m_data_in),
                                     .wren_a(wr_en),
                                     .q_a(q),
                                     .aclr0(1'b0),
                                     .aclr1(1'b0),
                                     .address_b(1'b1),
                                     .addressstall_a(1'b0),
                                     .addressstall_b(1'b0),
                                     .byteena_b(1'b1),
                                     .clock1(1'b1),
                                     .clocken0(1'b1),
                                     .clocken1(1'b1),
                                     .clocken2(1'b1),
                                     .clocken3(1'b1),
                                     .data_b(1'b1),
                                     .eccstatus(),
                                     .q_b(),
                                     .rden_a(1'b1),
                                     .rden_b(1'b1),
                                     .wren_b(1'b0));
defparam
        altsyncram_component.byte_size = 8,
        altsyncram_component.clock_enable_input_a = "BYPASS",
        altsyncram_component.clock_enable_output_a = "BYPASS",
        altsyncram_component.lpm_hint = "ENABLE_RUNTIME_MOD=NO",
        altsyncram_component.lpm_type = "altsyncram",
        altsyncram_component.numwords_a = depth,
        altsyncram_component.operation_mode = "SINGLE_PORT",
        altsyncram_component.outdata_aclr_a = "NONE",
        altsyncram_component.outdata_reg_a = "UNREGISTERED",
        altsyncram_component.power_up_uninitialized = "FALSE",
        altsyncram_component.read_during_write_mode_port_a = "NEW_DATA_NO_NBE_READ",
        altsyncram_component.widthad_a = $clog2(depth),
        altsyncram_component.width_a = 16,
        altsyncram_component.width_byteena_a = 2,
        altsyncram_component.init_file = "bios.mif";

endmodule
