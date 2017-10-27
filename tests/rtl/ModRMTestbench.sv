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

module ModRMTestbench(input logic clk,
                      input logic reset,
                      // Control.
                      input logic start,
                      output logic busy,
                      output logic complete,
                      input logic clear,
                      // Results
                      output logic [15:0] effective_address,
                      output logic [2:0] regnum,
                      output logic rm_is_reg,
                      output logic [2:0] rm_regnum,
                      output logic bp_as_base,
                      // Registers.
                      output logic [2:0] reg_sel[2],
                      input logic [15:0] regs[2],
                      // Fifo Read Port.
                      output logic fifo_rd_en,
                      input logic [7:0] fifo_rd_data,
                      input logic fifo_empty);

wire immed_is_8bit;
wire immed_start;
wire immed_complete;
wire immed_fifo_rd_en;
wire modrm_fifo_rd_en;
wire [15:0] immediate;

assign fifo_rd_en = immed_fifo_rd_en | modrm_fifo_rd_en;

ImmediateReader ir(.clk(clk),
                   .reset(reset),
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

ModRMDecode mrm(.clk(clk),
                .reset(reset),
                .start(start),
                .busy(busy),
                .complete(complete),
                .clear(clear),
                .effective_address(effective_address),
                .regnum(regnum),
                .rm_is_reg(rm_is_reg),
                .rm_regnum(rm_regnum),
                .bp_as_base(bp_as_base),
                .reg_sel(reg_sel),
                .regs(regs),
                .fifo_rd_en(modrm_fifo_rd_en),
                .fifo_rd_data(fifo_rd_data),
                .fifo_empty(fifo_empty),
                .immed_start(immed_start),
                .immed_complete(immed_complete),
                .immed_is_8bit(immed_is_8bit),
                .immediate(immediate));
endmodule
