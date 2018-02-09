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
module SPIMaster(input logic clk,
                 input logic reset,
                 input logic [8:0] divider,
                 input logic xfer_start,
                 output logic xfer_complete,
                 input logic [7:0] tx_data,
                 output logic [7:0] rx_data,
                 input logic miso,
                 output logic mosi,
                 output logic sclk);

reg [8:0] clk_counter = 9'b0;
reg [3:0] xfer_bit = 4'b0;
wire sclk_rising = ~sclk & ~|clk_counter;
wire sclk_falling = sclk & ~|clk_counter;
wire shift_complete = ~|clk_counter && xfer_bit[3] && sclk;

typedef enum logic [1:0] {
    STATE_IDLE,
    STATE_XFER,
    STATE_COMPLETE
} state_t;

state_t state, next_state;

always_comb begin
    case (state)
    STATE_IDLE: next_state = xfer_start ? STATE_XFER : STATE_IDLE;
    STATE_XFER: next_state = shift_complete ? STATE_COMPLETE : STATE_XFER;
    STATE_COMPLETE: next_state = STATE_IDLE;
    default: ;
    endcase

    if (reset)
        next_state = STATE_IDLE;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        xfer_complete <= 1'b0;
        rx_data <= 8'b0;
        mosi <= 1'b1;
    end else begin
        xfer_complete <= 1'b0;

        case (state)
        STATE_IDLE: begin
            mosi <= xfer_start ? tx_data[7] : 1'b1;
            xfer_bit <= 4'b0;
        end
        STATE_XFER: begin
            if (sclk_falling && ~xfer_bit[3])
                mosi <= tx_data[~xfer_bit[2:0]];
            else if (sclk_rising) begin
                xfer_bit <= xfer_bit + 1'b1;
                rx_data[~xfer_bit[2:0]] <= miso;
            end
        end
        STATE_COMPLETE: begin
            xfer_complete <= 1'b1;
            mosi <= 1'b1;
        end
        default: ;
        endcase
    end
end

always_ff @(posedge clk) begin
    case (state)
    STATE_IDLE: begin
        sclk <= 1'b0;
        clk_counter <= divider;
    end
    STATE_XFER: begin
        clk_counter <= clk_counter - 1'b1;
        if (~|clk_counter) begin
            clk_counter <= divider;
            sclk <= ~sclk;
        end
    end
    STATE_COMPLETE: begin
        sclk <= 1'b0;
    end
    default: ;
    endcase
end

always @(posedge clk)
    state <= next_state;

endmodule
