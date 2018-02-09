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
module Receiver(input logic clk,
                input logic reset,
                input logic clken,
                input logic rx,
                output logic rdy,
                input logic rdy_clr,
                output logic [7:0] data);

typedef enum logic [1:0] {
    RX_STATE_START,
    RX_STATE_DATA,
    RX_STATE_STOP
} state_t;

state_t state;
reg [3:0] sample;
reg [3:0] bitpos;
reg [7:0] scratch;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        sample <= 4'b0;
        bitpos <= 4'b0;
        scratch <= 8'b0;
        rdy <= 1'b0;
        state <= RX_STATE_START;
    end else begin
        if (rdy_clr)
            rdy <= 0;

        if (clken) begin
            case (state)
            RX_STATE_START: begin
                /*
                * Start counting from the first low sample, once we've
                * sampled a full bit, start collecting data bits.
                */
                if (!rx || sample != 0)
                    sample <= sample + 4'b1;

                if (sample == 15) begin
                    state <= RX_STATE_DATA;
                    bitpos <= 0;
                    sample <= 0;
                    scratch <= 0;
                end
            end
            RX_STATE_DATA: begin
                sample <= sample + 4'b1;
                if (sample == 4'h8) begin
                    scratch[bitpos[2:0]] <= rx;
                    bitpos <= bitpos + 4'b1;
                end
                if (bitpos == 8 && sample == 15)
                    state <= RX_STATE_STOP;
            end
            RX_STATE_STOP: begin
                /*
                * Our baud clock may not be running at exactly the
                * same rate as the transmitter.  If we thing that
                * we're at least half way into the stop bit, allow
                * transition into handling the next start bit.
                */
                if (sample == 15 || (sample >= 8 && !rx)) begin
                    state <= RX_STATE_START;
                    data <= scratch;
                    rdy <= 1'b1;
                    sample <= 0;
                end else begin
                    sample <= sample + 4'b1;
                end
            end
            default: begin
                state <= RX_STATE_START;
            end
            endcase
        end
    end
end

endmodule
