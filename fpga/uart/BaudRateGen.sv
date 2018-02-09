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
module BaudRateGen #(parameter clk_freq = 50000000)
                    (input logic clk,
                     input logic reset,
                     output logic rxclk_en,
                     output logic txclk_en);

localparam RX_ACC_MAX = clk_freq / (115200 * 16);
localparam TX_ACC_MAX = clk_freq / 115200;
localparam RX_ACC_WIDTH = $clog2(RX_ACC_MAX);
localparam TX_ACC_WIDTH = $clog2(TX_ACC_MAX);
reg [RX_ACC_WIDTH - 1:0] rx_acc = 0;
reg [TX_ACC_WIDTH - 1:0] tx_acc = 0;

assign rxclk_en = (rx_acc == 5'd0);
assign txclk_en = (tx_acc == 9'd0);

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        rx_acc <= {RX_ACC_WIDTH{1'b0}};
    end else begin
        if (rx_acc == RX_ACC_MAX[RX_ACC_WIDTH - 1:0])
            rx_acc <= {RX_ACC_WIDTH{1'b0}};
        else
            rx_acc <= rx_acc + 1'b1;
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        tx_acc <= {TX_ACC_WIDTH{1'b0}};
    end else begin
        if (tx_acc == TX_ACC_MAX[TX_ACC_WIDTH - 1:0])
            tx_acc <= {TX_ACC_WIDTH{1'b0}};
        else
            tx_acc <= tx_acc + 1'b1;
    end
end

endmodule
