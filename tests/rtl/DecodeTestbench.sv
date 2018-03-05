// Copyright Jamie Iles, 2018
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

module DecodeTestbench(input logic clk,
                       input logic reset,
                       input logic stall,
                       input logic nearly_full,
                       // Control.
                       output logic complete,
                       // Results
                       output logic [7:0] opcode,
                       output logic [7:0] mod_rm,
                       output logic [15:0] displacement,
                       output logic [3:0] length,
                       output logic lock,
                       output logic [1:0] rep,
                       output logic [1:0] segment,
                       output logic has_segment_override,
                       output logic [15:0] immed1,
                       output logic [15:0] immed2,
                       // Fifo Read Port.
                       output logic fifo_rd_en,
                       input logic [7:0] fifo_rd_data,
                       input logic fifo_empty);

wire immed_is_8bit;
wire immed_start;
wire immed_complete;
wire immed_fifo_rd_en;
wire [15:0] immediate;
wire decode_fifo_rd_en;

// verilator lint_off UNUSED
Instruction instruction;
// verilator lint_on UNUSED

assign fifo_rd_en = immed_fifo_rd_en | decode_fifo_rd_en;
assign opcode = instruction.opcode;
assign mod_rm = instruction.mod_rm;
assign displacement = instruction.displacement;
assign length = instruction.length;
assign lock = instruction.lock;
assign rep = instruction.rep;
assign segment = instruction.segment;
assign has_segment_override = instruction.has_segment_override;
assign immed1 = instruction.immediates[0];
assign immed2 = instruction.immediates[1];

InsnDecoder InsnDecoder(.*,
                        .flush(1'b0),
                        .fifo_rd_en(decode_fifo_rd_en));

ImmediateReader ir(.clk(clk),
                   .reset(reset),
                   .flush(1'b0),
                   .start(immed_start),
                   // verilator lint_off PINCONNECTEMPTY
                   .busy(),
                   // verilator lint_on PINCONNECTEMPTY
                   .complete(immed_complete),
                   .is_8bit(immed_is_8bit),
                   .immediate(immediate),
                   .fifo_rd_en(immed_fifo_rd_en),
                   .fifo_rd_data(fifo_rd_data),
                   .fifo_empty(fifo_empty));

endmodule
