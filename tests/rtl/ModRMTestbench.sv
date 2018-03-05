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
                      input logic clear,
                      input logic [7:0] modrm,
                      input logic [15:0] displacement,
                      input logic [15:0] si,
                      input logic [15:0] di,
                      input logic [15:0] bp,
                      input logic [15:0] bx,
                      // Results
                      output logic [15:0] effective_address,
                      output logic [2:0] regnum,
                      output logic rm_is_reg,
                      output logic [2:0] rm_regnum,
                      output logic bp_as_base);

ModRMDecode mrm(.clk(clk),
                .reset(reset),
                .start(start),
                .clear(clear),
                .si(si),
                .di(di),
                .bp(bp),
                .bx(bx),
                .modrm(modrm),
                .displacement(displacement),
                .effective_address(effective_address),
                .regnum(regnum),
                .rm_is_reg(rm_is_reg),
                .rm_regnum(rm_regnum),
                .bp_as_base(bp_as_base));

endmodule
