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
module ModRMDecode(input logic clk,
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

wire [1:0] _mod = modrm[7:6];
wire [2:0] _reg = modrm[5:3];
wire [2:0] _rm  = modrm[2:0];

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        bp_as_base <= 1'b0;
        rm_is_reg <= 1'b0;
        regnum <= 3'b0;
        rm_regnum <= 3'b0;
    end else begin
        if (clear)
            bp_as_base <= 1'b0;
        if (start) begin
            regnum <= _reg;
            rm_regnum <= _rm;
            rm_is_reg <= _mod == 2'b11;

            case (_mod)
            2'b00: bp_as_base <= (_rm == 3'b010 || _rm == 3'b011);
            2'b01: bp_as_base <= (_rm == 3'b010 || _rm == 3'b011 || _rm == 3'b110);
            2'b10: bp_as_base <= (_rm == 3'b010 || _rm == 3'b011 || _rm == 3'b110);
            default: bp_as_base <= 1'b0;
            endcase
        end
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        effective_address <= 16'b0;
    else begin
        if (start) begin
            effective_address <= 16'b0;

            case (_mod)
            2'b00: begin
                case (_rm)
                3'd0: effective_address <= bx + si;
                3'd1: effective_address <= bx + di;
                3'd2: effective_address <= bp + si;
                3'd3: effective_address <= bp + di;
                3'd4: effective_address <= si;
                3'd5: effective_address <= di;
                3'd6: effective_address <= displacement;
                3'd7: effective_address <= bx;
                endcase
            end
            2'b01, 2'b10: begin
                case (_rm)
                3'd0: effective_address <= bx + si + displacement;
                3'd1: effective_address <= bx + di + displacement;
                3'd2: effective_address <= bp + si + displacement;
                3'd3: effective_address <= bp + di + displacement;
                3'd4: effective_address <= si + displacement;
                3'd5: effective_address <= di + displacement;
                3'd6: effective_address <= bp + displacement;
                3'd7: effective_address <= bx + displacement;
                endcase
            end
            2'b11: effective_address <= 16'b0;
            endcase
        end
    end
end

endmodule
