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
module LEDSRegister(input logic clk,
                    input logic reset,
                    output logic [`CONFIG_NUM_LEDS-1:0] leds_val,
                    input logic cs,
                    input logic [15:0] data_m_data_in,
                    output logic [15:0] data_m_data_out,
                    input logic data_m_access,
                    output logic data_m_ack,
                    input logic data_m_wr_en,
                    input logic [1:0] data_m_bytesel);

always_ff @(posedge clk or posedge reset)
    if (reset)
        leds_val <= `CONFIG_NUM_LEDS'b0;
    else begin
        if (`CONFIG_NUM_LEDS > 8 && cs && data_m_access && data_m_bytesel[1]
            && data_m_wr_en)
            leds_val[`CONFIG_NUM_LEDS-1:8] <= data_m_data_in[`CONFIG_NUM_LEDS-1:8];
        if (cs && data_m_access && data_m_bytesel[0] && data_m_wr_en)
            leds_val[7:0] <= data_m_data_in[7:0];
    end

always_ff @(posedge clk)
    data_m_data_out <= cs & data_m_access ? 16'(leds_val) : 16'b0;

always_ff @(posedge clk)
    data_m_ack <= cs & data_m_access;

endmodule
