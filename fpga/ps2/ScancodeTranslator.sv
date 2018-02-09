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
module ScancodeTranslator(input logic [7:0] scancode_in,
                          output logic [7:0] scancode_out);

always_comb begin
    case (scancode_in)
    8'h1c: scancode_out = 8'h1E;
    8'h32: scancode_out = 8'h30;
    8'h21: scancode_out = 8'h2E;
    8'h23: scancode_out = 8'h20;
    8'h24: scancode_out = 8'h12;
    8'h2b: scancode_out = 8'h21;
    8'h34: scancode_out = 8'h22;
    8'h33: scancode_out = 8'h23;
    8'h43: scancode_out = 8'h17;
    8'h3b: scancode_out = 8'h24;
    8'h42: scancode_out = 8'h25;
    8'h4b: scancode_out = 8'h26;
    8'h3a: scancode_out = 8'h32;
    8'h31: scancode_out = 8'h31;
    8'h44: scancode_out = 8'h18;
    8'h4d: scancode_out = 8'h19;
    8'h15: scancode_out = 8'h10;
    8'h2d: scancode_out = 8'h13;
    8'h1b: scancode_out = 8'h1F;
    8'h2c: scancode_out = 8'h14;
    8'h3c: scancode_out = 8'h16;
    8'h2a: scancode_out = 8'h2F;
    8'h1d: scancode_out = 8'h11;
    8'h22: scancode_out = 8'h2D;
    8'h35: scancode_out = 8'h15;
    8'h1a: scancode_out = 8'h2C;
    8'h16: scancode_out = 8'h02;
    8'h1e: scancode_out = 8'h03;
    8'h26: scancode_out = 8'h04;
    8'h25: scancode_out = 8'h05;
    8'h2e: scancode_out = 8'h06;
    8'h36: scancode_out = 8'h07;
    8'h3d: scancode_out = 8'h08;
    8'h3e: scancode_out = 8'h09;
    8'h46: scancode_out = 8'h0A;
    8'h45: scancode_out = 8'h0B;
    8'h4e: scancode_out = 8'h0C;
    8'h55: scancode_out = 8'h0D;
    8'h54: scancode_out = 8'h1A;
    8'h5b: scancode_out = 8'h1B;
    8'h4c: scancode_out = 8'h27;
    8'h52: scancode_out = 8'h28;
    8'h0e: scancode_out = 8'h29;
    8'h5d: scancode_out = 8'h2B;
    8'h41: scancode_out = 8'h33;
    8'h49: scancode_out = 8'h34;
    8'h4a: scancode_out = 8'h35;
    8'h05: scancode_out = 8'h3B;
    8'h06: scancode_out = 8'h3C;
    8'h04: scancode_out = 8'h3D;
    8'h0c: scancode_out = 8'h3E;
    8'h03: scancode_out = 8'h3F;
    8'h0b: scancode_out = 8'h40;
    8'h83: scancode_out = 8'h41;
    8'h0a: scancode_out = 8'h42;
    8'h01: scancode_out = 8'h43;
    8'h09: scancode_out = 8'h44;
    8'h78: scancode_out = 8'h85;
    8'h07: scancode_out = 8'h86;
    8'h66: scancode_out = 8'h0E;
    8'h5a: scancode_out = 8'h1C;
    8'h76: scancode_out = 8'h01;
    8'h29: scancode_out = 8'h39;
    8'h0d: scancode_out = 8'h0F;
    8'h71: scancode_out = 8'h53;
    8'h72: scancode_out = 8'h50;
    8'h69: scancode_out = 8'h4F;
    8'h6c: scancode_out = 8'h47;
    8'h70: scancode_out = 8'h52;
    8'h6b: scancode_out = 8'h4B;
    8'h7a: scancode_out = 8'h51;
    8'h7d: scancode_out = 8'h49;
    8'h74: scancode_out = 8'h4D;
    8'h75: scancode_out = 8'h48;
    8'h12: scancode_out = 8'h2a;
    8'h14: scancode_out = 8'h1d;
    8'h11: scancode_out = 8'h38;
    default: scancode_out = 8'h00;
    endcase
end

endmodule
