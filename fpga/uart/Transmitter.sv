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
module Transmitter(input logic clk,
                   input logic reset,
                   input logic clken,
                   input logic [7:0] din,
                   input logic wr_en,
                   output logic tx,
                   output logic tx_busy);

typedef enum logic [1:0] {
    STATE_IDLE,
    STATE_START,
    STATE_DATA,
    STATE_STOP
} state_t;

reg [7:0] data;
reg [2:0] bitpos;
state_t state;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        state <= STATE_IDLE;
        data <= 8'b0;
        bitpos <= 3'b0;
        tx <= 1'b1;
    end else begin
        case (state)
        STATE_IDLE: begin
            if (wr_en) begin
                state <= STATE_START;
                data <= din;
                bitpos <= 3'h0;
            end
        end
        STATE_START: begin
            if (clken) begin
                tx <= 1'b0;
                state <= STATE_DATA;
            end
        end
        STATE_DATA: begin
            if (clken) begin
                if (bitpos == 3'h7)
                    state <= STATE_STOP;
                else
                    bitpos <= bitpos + 3'h1;
                tx <= data[bitpos];
            end
        end
        STATE_STOP: begin
            if (clken) begin
                tx <= 1'b1;
                state <= STATE_IDLE;
            end
        end
        default: begin
            tx <= 1'b1;
            state <= STATE_IDLE;
        end
        endcase
    end
end

assign tx_busy = (state != STATE_IDLE);

endmodule
