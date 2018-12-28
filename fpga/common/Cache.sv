// Copyright Jamie Iles, 2018
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

//verilator lint_off UNUSED
`default_nettype none
module Cache(input logic clk,
             input logic reset,
             input logic enabled,
             // Frontend
             input logic [19:1] c_addr,
             output logic [15:0] c_data_in,
             input logic [15:0] c_data_out,
             input logic c_access,
             output logic c_ack,
             input logic c_wr_en,
             input logic [1:0] c_bytesel,
             // Backend
             output logic [19:1] m_addr,
             input logic [15:0] m_data_in,
             output logic [15:0] m_data_out,
             output logic m_access,
             input logic m_ack,
             output logic m_wr_en,
             output logic [1:0] m_bytesel);

parameter lines = 512;

localparam line_size = 8;
localparam index_bits = $clog2(lines);
localparam tag_bits = 19 - 3 - index_bits;
localparam index_start = 4;
localparam index_end = 4 + index_bits - 1;
localparam tag_start = 4 + index_bits;

reg [19:1] c_m_addr;
reg [15:0] c_m_data_out;
reg [2:0] line_idx;
wire [19:tag_start] tag;
reg [7:0] line_valid;
reg busy;
reg flushing;
wire dirty;
wire write_line = m_ack && !flushing;
reg [index_end-1:0] line_address;
reg [19:1] latched_address, fetch_address;
reg updating;
reg accessing;
wire tags_match = tag == fetch_address[19:tag_start];
wire filling_current = fetch_address[19:index_start] == latched_address[19:index_start];
wire hit = accessing && ((valid && tags_match) ||
    (busy && filling_current && line_valid[fetch_address[3:1]]));

wire [15:0] c_q;
assign c_data_in = enabled ? (c_ack ? c_q : 16'b0) : m_data_in;
assign c_ack = enabled ? accessing & !flushing & hit : m_ack;

assign m_addr = enabled ? c_m_addr : c_addr;
assign m_wr_en = enabled ? flushing & ~m_ack : c_wr_en;
assign m_access = enabled ? busy & ~m_ack : c_access;
assign m_bytesel = enabled ? 2'b11 : c_bytesel;
assign m_data_out = enabled ? c_m_data_out : c_data_out;

wire do_flush = updating && ~hit && !busy && !flushing && dirty;
wire do_fill = updating && ~hit && !busy && !flushing && !dirty;

wire write_tag = do_fill;
wire valid;
wire write_valid = do_flush | write_tag | (~flushing && line_idx == 3'b111 && m_ack);

DPRam #(.words(lines),
        .width(tag_bits))
      TagRam(.clk(clk),
             .addr_a(c_addr[index_end:index_start]),
             .wr_en_a(1'b0),
             .wdata_a({tag_bits{1'b0}}),
             .q_a(tag),
             .addr_b(latched_address[index_end:index_start]),
             .wr_en_b(write_tag),
             .wdata_b(latched_address[19:tag_start]),
             // verilator lint_off PINCONNECTEMPTY
             .q_b());
             // verilator lint_on PINCONNECTEMPTY

DPRam #(.words(lines),
        .width(1))
      ValidRam(.clk(clk),
               .addr_a(c_addr[index_end:index_start]),
               .wr_en_a(1'b0),
               .wdata_a(1'b0),
               .q_a(valid),
               .addr_b(latched_address[index_end:index_start]),
               .wr_en_b(write_valid),
               .wdata_b(do_flush ? 1'b0 : (~flushing && line_idx == 3'b111)),
               // verilator lint_off PINCONNECTEMPTY
               .q_b());
               // verilator lint_on PINCONNECTEMPTY

DPRam #(.words(lines),
        .width(1))
      DirtyRam(.clk(clk),
               .addr_a(c_addr[index_end:index_start]),
               .wr_en_a(c_ack & c_wr_en),
               .wdata_a(1'b1),
               // verilator lint_off PINCONNECTEMPTY
               .q_a(dirty),
               // verilator lint_on PINCONNECTEMPTY
               .addr_b(latched_address[index_end:index_start]),
               .wr_en_b(do_flush),
               .wdata_b(1'b0),
               // verilator lint_off PINCONNECTEMPTY
               .q_b());
               // verilator lint_on PINCONNECTEMPTY

BlockRam #(.words(lines * line_size))
         LineRAM(.clk(clk),
                 .addr_a(c_addr[index_end:1]),
                 .wr_en_a(c_ack && c_wr_en && !flushing && hit),
                 .wdata_a(c_data_out),
                 .be_a(c_bytesel),
                 .q_a(c_q),
                 .addr_b(line_address),
                 .wr_en_b(write_line),
                 .wdata_b(m_data_in),
                 .q_b(c_m_data_out),
                 .be_b(2'b11));

task flush_line;
begin
    c_m_addr <= {tag, latched_address[index_end:index_start], 3'b0};
    busy <= 1'b1;
    flushing <= 1'b1;
end
endtask

task fill_line;
begin
    c_m_addr <= c_addr;
    busy <= 1'b1;
    line_valid <= 8'b0;
end
endtask

// No reset: the CPU isn't cache coherent so we need to preserve state across
// reset
always_ff @(posedge reset)
    ;

always_ff @(posedge clk)
    accessing <= c_access;

always_comb begin
    if (m_ack && flushing)
        line_address = {latched_address[index_end:index_start], c_m_addr[3:1] + 1'b1};
    else if (~hit && !flushing && !busy && dirty)
        line_address = {latched_address[index_end:index_start], 3'b0};
    else
        line_address = {latched_address[index_end:index_start], c_m_addr[3:1]};
end

always_ff @(posedge clk) begin
    if (!busy && !flushing)
        latched_address <= c_addr;
    fetch_address <= c_addr;
end

always_ff @(posedge clk) begin
    if (enabled && !busy && !flushing && c_access)
        updating <= 1'b1;
    if (updating && !(do_flush || flushing))
        updating <= 1'b0;
end

always_ff @(posedge clk) begin
    if (enabled && m_ack) begin
        c_m_addr <= {c_m_addr[19:4], c_m_addr[3:1] + 1'b1};
        line_idx <= line_idx + 1'b1;
        if (!flushing)
            line_valid[c_m_addr[3:1]] <= 1'b1;
        if (line_idx == 3'b111) begin
            busy <= 1'b0;
            if (flushing)
                flushing <= 1'b0;
        end
    end else if (enabled && do_flush)
        flush_line();
    else if (enabled && do_fill)
        fill_line();
end

endmodule
