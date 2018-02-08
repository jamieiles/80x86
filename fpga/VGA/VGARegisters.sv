// Copyright Jamie Iles, 2017, 2018
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
                    input logic vga_clk,
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
                    // VGA clock domain signals
                    output logic cursor_enabled,
                    output logic graphics_enabled,
                    output logic [3:0] background_color,
                    output logic bright_colors,
                    output logic palette_sel,
                    output logic [14:0] cursor_pos,
                    output logic [2:0] cursor_scan_start,
                    output logic [2:0] cursor_scan_end);

wire reg_access = cs & data_m_access;
wire sel_index  = reg_access & data_m_addr[3:1] == 3'b010 & data_m_bytesel[0];
wire sel_value  = reg_access & data_m_addr[3:1] == 3'b010 & data_m_bytesel[1];
wire sel_mode   = reg_access & data_m_addr[3:1] == 3'b100 & data_m_bytesel[0];
wire sel_color  = reg_access & data_m_addr[3:1] == 3'b100 & data_m_bytesel[1];
wire sel_status = reg_access & data_m_addr[3:1] == 3'b101 & data_m_bytesel[0];

reg [3:0] active_index;
wire [3:0] index = data_m_wr_en & sel_index ? data_m_data_in[3:0] : active_index;
wire [7:0] index_value;

reg [1:0] cursor_mode;
reg display_enabled;
reg text_enabled;
reg sys_graphics_enabled;
reg sys_bright_colors;
reg sys_cursor_enabled;
reg sys_palette_sel;

reg [14:0] sys_cursor_pos;
wire load_vga_cursor;
wire rdy_vga_cursor;
wire [14:0] vga_cursor;

wire [2:0] sys_cursor_scan_start;
wire load_cursor_scan_start;
wire [2:0] vga_cursor_scan_start;

wire [2:0] sys_cursor_scan_end;
wire load_cursor_scan_end;
wire [2:0] vga_cursor_scan_end;

wire [3:0] sys_background_color;
wire load_background_color;
wire [3:0] vga_background_color;

reg vga_send;

always_ff @(posedge clk)
    vga_send <= rdy_vga_cursor;

wire hsync;
wire vsync;

BitSync         HsyncSync(.clk(clk),
                          .reset(reset),
                          .d(vga_hsync),
                          .q(hsync));
BitSync         VsyncSync(.clk(clk),
                          .reset(reset),
                          .d(vga_vsync),
                          .q(vsync));
BitSync         GraphicsEnabledSync(.clk(vga_clk),
                                    .reset(reset),
                                    .d(sys_graphics_enabled),
                                    .q(graphics_enabled));
BitSync         BrightColorsSync(.clk(vga_clk),
                                 .reset(reset),
                                 .d(sys_bright_colors),
                                 .q(bright_colors));
BitSync         CursorEnabledSync(.clk(vga_clk),
                                  .reset(reset),
                                  .d(sys_cursor_enabled),
                                  .q(cursor_enabled));
BitSync         PaletteSelSync(.clk(vga_clk),
                               .reset(reset),
                               .d(sys_palette_sel),
                               .q(palette_sel));
MCP             #(.width(15),
                  .reset_val(15'b0))
                CursorMCP(.reset(reset),
                          .clk_a(clk),
                          .a_ready(rdy_vga_cursor),
                          .a_datain(sys_cursor_pos),
                          .a_send(vga_send),
                          .clk_b(vga_clk),
                          .b_data(vga_cursor),
                          .b_load(load_vga_cursor));

MCP             #(.width(3),
                  .reset_val(3'b0))
                CursorScanStartMCP(.reset(reset),
                                   .clk_a(clk),
                                   .a_ready(),
                                   .a_datain(sys_cursor_scan_start),
                                   .a_send(vga_send),
                                   .clk_b(vga_clk),
                                   .b_data(vga_cursor_scan_start),
                                   .b_load(load_cursor_scan_start));
MCP             #(.width(3),
                  .reset_val(3'b0))
                CursorScanEndMCP(.reset(reset),
                                 .clk_a(clk),
                                 .a_ready(),
                                 .a_datain(sys_cursor_scan_end),
                                 .a_send(vga_send),
                                 .clk_b(vga_clk),
                                 .b_data(vga_cursor_scan_end),
                                 .b_load(load_cursor_scan_end));
MCP             #(.width(4),
                  .reset_val(4'b0))
                BackgroundColorMCP(.reset(reset),
                                   .clk_a(clk),
                                   .a_ready(),
                                   .a_datain(sys_background_color),
                                   .a_send(vga_send),
                                   .clk_b(vga_clk),
                                   .b_data(vga_background_color),
                                   .b_load(load_background_color));

always_ff @(posedge vga_clk) begin
    if (load_vga_cursor)
        cursor_pos <= vga_cursor;
    if (load_cursor_scan_start)
        cursor_scan_start <= vga_cursor_scan_start;
    if (load_cursor_scan_end)
        cursor_scan_end <= vga_cursor_scan_end;
    if (load_background_color)
        background_color <= vga_background_color;
end

wire [7:0] status = {4'b0, vga_vsync, 2'b0, (~vsync | ~hsync)};

always_ff @(posedge clk)
    sys_cursor_enabled <= cursor_mode != 2'b01;

always_ff @(posedge clk) begin
    if (data_m_wr_en & sel_value) begin
        case (index)
        4'ha: {cursor_mode, sys_cursor_scan_start} <=
            {data_m_data_in[13:12], data_m_data_in[10:8]};
        4'hb: sys_cursor_scan_end <= data_m_data_in[10:8];
        4'he: sys_cursor_pos[14:8] <= data_m_data_in[14:8];
        4'hf: sys_cursor_pos[7:0] <= data_m_data_in[15:8];
        default: ;
        endcase
    end
end

always_ff @(posedge clk) begin
    if (data_m_wr_en & sel_color)
        {sys_palette_sel, sys_bright_colors, sys_background_color} <=
            data_m_data_in[13:8];
end

always_ff @(posedge clk or posedge reset)
    if (reset) begin
        sys_graphics_enabled <= 1'b0;
        text_enabled <= 1'b0;
    end else if (sel_mode && data_m_wr_en)
        {display_enabled, sys_graphics_enabled, text_enabled} <=
            {data_m_data_in[3], data_m_data_in[1], data_m_data_in[0]};

always_ff @(posedge clk)
    if (sel_index & data_m_wr_en)
        active_index <= data_m_data_in[3:0];

always_comb begin
    case (index)
    4'ha: index_value = {2'b0, cursor_mode, 1'b0, sys_cursor_scan_start};
    4'hb: index_value = {5'b0, sys_cursor_scan_end};
    4'he: index_value = {2'b0, sys_cursor_pos[13:8]};
    4'hf: index_value = sys_cursor_pos[7:0];
    default: index_value = 8'b0;
    endcase
end

always_ff @(posedge clk) begin
    data_m_data_out <= 16'b0;

    if (!data_m_wr_en) begin
        if (sel_index)
            data_m_data_out[7:0] <= {4'b0, active_index};
        if (sel_mode)
            data_m_data_out[7:0] <= {4'b0, display_enabled, 1'b0,
                                     sys_graphics_enabled, text_enabled};
        if (sel_status)
            data_m_data_out[7:0] <= status;

        if (sel_value)
            data_m_data_out[15:8] <= index_value;
        if (sel_color)
            data_m_data_out[15:8] <= {2'b0, sys_palette_sel, sys_bright_colors,
                                      sys_background_color};
    end
end

always_ff @(posedge clk)
    data_m_ack <= data_m_access && cs;

endmodule
