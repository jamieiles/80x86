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

// Multi-cycle-path formulation with feedback as described in "Clock Domain
// Crossing (CDC) Design & Verification Techniques Using SystemVerilog" by
// Clifford E. Cummings.
`default_nettype none
module MCP #(parameter width = 8,
             parameter reset_val = 8'b0)
            (input logic reset,
             input logic clk_a,
             output logic a_ready,
             input logic a_send,
             input logic [width-1:0] a_datain,
             input logic clk_b,
             output logic [width-1:0] b_data,
             output logic b_load);

logic [width-1:0] tx_sample;
assign a_ready = a_ack | ~tx_busy;
reg a_en;
wire a_ack;
reg tx_busy;
wire a_load = a_send & a_ready;
wire b_ack;
assign b_data = tx_sample;

SyncPulse       BLoadPulse(.clk(clk_b),
                           .d(a_en),
                           .p(b_load),
                           .q(b_ack));

SyncPulse       AAckPulse(.clk(clk_a),
                          .d(b_ack),
                          .p(a_ack),
                          // verilator lint_off PINCONNECTEMPTY
                          .q()
                          // verilator lint_on PINCONNECTEMPTY
                          );

always_ff @(posedge clk_a)
    a_en <= a_en ^ a_load;

always_ff @(posedge clk_a or posedge reset) begin
    if (reset)
        tx_busy <= 1'b0;
    else begin
        if (a_ack)
            tx_busy <= 1'b0;
        if (a_send)
            tx_busy <= 1'b1;
    end
end

always_ff @(posedge clk_a or posedge reset)
    if (reset)
        tx_sample <= reset_val;
    else if (a_load)
        tx_sample <= a_datain;

endmodule
