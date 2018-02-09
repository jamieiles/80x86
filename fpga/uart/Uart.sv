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
module Uart #(parameter clk_freq = 50000000)
             (input logic clk,
              input logic reset,
              input logic [7:0] din,
              input logic wr_en,
              output logic tx,
              output logic tx_busy,
              input logic rx,
              output logic rdy,
              input logic rdy_clr,
              output logic [7:0] dout);

wire rxclk_en, txclk_en;

BaudRateGen #(.clk_freq(clk_freq))
            BaudRateGen(.*);

Transmitter Transmitter(.clken(txclk_en),
                        .*);

Receiver Receiver(.clken(rxclk_en),
                  .data(dout),
                  .*);

endmodule
