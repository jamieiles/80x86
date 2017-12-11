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

module VGARegisters(input logic clk,
                    input logic reset,
                    // Bus
                    input logic cs,
                    input logic [19:1] data_m_addr,
                    input logic [15:0] data_m_data_in,
                    output logic [15:0] data_m_data_out,
                    input logic [1:0] data_m_bytesel,
                    input logic data_m_wr_en,
                    input logic data_m_access,
                    output logic data_m_ack,
                    // VGA
                    input logic vga_vsync,
                    input logic vga_hsync,
                    output logic cursor_enabled,
                    output logic [14:0] cursor_pos,
                    output logic [2:0] cursor_scan_start,
                    output logic [2:0] cursor_scan_end);

wire reg_access = cs & data_m_access;
wire sel_index  = reg_access & data_m_addr[3:1] == 3'b010 & data_m_bytesel[0];
wire sel_value  = reg_access & data_m_addr[3:1] == 3'b010 & data_m_bytesel[1];
wire sel_mode   = reg_access & data_m_addr[3:1] == 3'b011 & data_m_bytesel[0];
wire sel_color  = reg_access & data_m_addr[3:1] == 3'b011 & data_m_bytesel[1];
wire sel_status = reg_access & data_m_addr[3:1] == 3'b101 & data_m_bytesel[0];

reg [3:0] active_index;
wire [3:0] index = data_m_wr_en & sel_index ? data_m_data_in[3:0] : active_index;
wire [7:0] index_value;

reg [1:0] cursor_mode;

wire [7:0] status = {4'b0, vga_vsync, 2'b0, (~vga_vsync | ~vga_hsync)};

assign cursor_enabled = cursor_mode != 2'b01;

always_ff @(posedge clk) begin
    if (data_m_wr_en & sel_value) begin
        case (index)
        4'ha: {cursor_mode, cursor_scan_start} <=
            {data_m_data_in[13:12], data_m_data_in[10:8]};
        4'hb: cursor_scan_end <= data_m_data_in[10:8];
        4'he: cursor_pos[14:8] <= data_m_data_in[14:8];
        4'hf: cursor_pos[7:0] <= data_m_data_in[15:8];
        default: ;
        endcase
    end
end

always_ff @(posedge clk)
    if (sel_index & data_m_wr_en)
        active_index <= data_m_data_in[3:0];

always_comb begin
    case (index)
    4'ha: index_value = {2'b0, cursor_mode, 1'b0, cursor_scan_start};
    4'hb: index_value = {5'b0, cursor_scan_start};
    4'he: index_value = {2'b0, cursor_pos[13:8]};
    4'hf: index_value = cursor_pos[7:0];
    default: index_value = 8'b0;
    endcase
end

always_ff @(posedge clk) begin
    data_m_data_out <= 16'b0;

    if (!data_m_wr_en) begin
        if (sel_index)
            data_m_data_out[7:0] <= {4'b0, active_index};
        if (sel_mode)
            data_m_data_out[7:0] <= 8'b0;
        if (sel_status)
            data_m_data_out[7:0] <= status;

        if (sel_value)
            data_m_data_out[15:8] <= index_value;
        if (sel_color)
            data_m_data_out[15:8] <= 8'b0;
    end
end

always_ff @(posedge clk)
    data_m_ack <= data_m_access && cs;

endmodule
