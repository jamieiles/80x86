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
module SPIPorts(input logic clk,
                input logic reset,
                input logic cs,
                input logic [1:1] data_m_addr,
                input logic [15:0] data_m_data_in,
                output logic [15:0] data_m_data_out,
                input logic [1:0] data_m_bytesel,
                input logic data_m_wr_en,
                input logic data_m_access,
                output logic data_m_ack,
                input logic miso,
                output logic mosi,
                output logic sclk,
                output logic ncs);

wire access_control = ~data_m_addr[1] & cs & data_m_access;
wire access_xfer = data_m_addr[1] & cs & data_m_access;

reg [8:0] divider;

reg xfer_start;
wire xfer_complete;
reg xfer_busy;
reg [7:0] tx_data;
wire [7:0] rx_data;

SPIMaster SPIMaster(.*);

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        divider <= 9'b0;
        ncs <= 1'b0;
    end else if (access_control & data_m_wr_en) begin
        {ncs, divider} <= {data_m_data_in[9], data_m_data_in[8:0]};
    end
end

always_ff @(posedge clk)
    if (access_control && !data_m_wr_en)
        data_m_data_out <= {6'b0, ncs, divider};
    else if (access_xfer & ~data_m_wr_en)
        data_m_data_out <= {7'b0, xfer_busy, rx_data};
    else
        data_m_data_out <= 16'b0;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        tx_data <= 8'b0;
        xfer_start <= 1'b0;
        xfer_busy <= 1'b0;
    end else begin
        xfer_start <= access_xfer & data_m_wr_en;

        if (xfer_complete)
            xfer_busy <= 1'b0;
        if (access_xfer & data_m_wr_en) begin
            xfer_busy <= 1'b1;
            tx_data <= data_m_data_in[7:0];
        end
    end
end

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

endmodule
