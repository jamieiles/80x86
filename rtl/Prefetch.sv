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

// Prefetcher
//
// This module is responsible for fetching bytes from the memory and pushing
// them into the instruction stream FIFO.  The CS is stored outside of the
// Prefetch unit and passed in to be combined with the internal IP which is
// wired out.  This means that IP wrapping works correctly, and can be updated
// under external control.  The fetching can be stalled when servicing
// a branch, updating the IP will flush the FIFO.
`default_nettype none
module Prefetch(input logic clk,
                input logic reset,
                // Address control.
                input logic [15:0] new_cs,
                input logic [15:0] new_ip,
                input logic load_new_ip,
                // FIFO write port.
                output logic fifo_wr_en,
                output logic [7:0] fifo_wr_data,
                output logic fifo_reset,
                input logic fifo_full,
                // Memory port.
                output logic mem_access,
                input logic mem_ack,
                output logic [19:1] mem_address,
                input logic [15:0] mem_data);

reg [15:0] next_fetch_address, fetch_address;
reg [15:0] next_cs, cs;
reg abort_cur;
reg [7:0] fetched_high_byte;
reg write_second;
reg [15:0] next_sequential_fetch;

wire should_write_second_byte = mem_ack && !abort_cur && !fetch_address[0] && !fifo_reset;

// verilator lint_off UNUSED
wire [15:0] next_address = mem_ack ? next_sequential_fetch : fetch_address;
// verilator lint_on UNUSED

wire [19:1] _mem_address = {cs, 3'b0} + {4'b0, next_address[15:1]};
wire _mem_access = !reset && !fifo_full && !mem_ack && !write_second && ~(~mem_access && load_new_ip);

reg mem_access2;
assign mem_access = mem_access2 & !mem_ack;

always_ff @(posedge clk) begin
mem_address <= _mem_address;
mem_access2 <= _mem_access;
end

assign fifo_wr_en = !abort_cur && !load_new_ip && (mem_ack || write_second);
assign fifo_reset = load_new_ip | (abort_cur & mem_ack);
assign fifo_wr_data = mem_ack ?
    (fetch_address[0] ? mem_data[15:8] : mem_data[7:0]) : fetched_high_byte;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        abort_cur <= 1'b0;
    end else begin
        if (abort_cur && mem_ack)
            abort_cur <= 1'b0;
        else if (mem_access && load_new_ip)
            abort_cur <= 1'b1;
    end
end

always_ff @(posedge clk or posedge reset)
    write_second <= reset ? 1'b0 : should_write_second_byte;

always_ff @(posedge clk or posedge reset)
    if (reset) begin
        cs <= 16'hffff;
    end else begin
        if (abort_cur && mem_ack)
            cs <= next_cs;
        else if (load_new_ip && !mem_access)
            cs <= new_cs;
    end

always_ff @(posedge clk or posedge reset)
    if (reset)
        next_cs <= 16'hffff;
    else if (load_new_ip)
        next_cs <= new_cs;

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        fetch_address <= 16'b0;
    else if (abort_cur && mem_ack)
        fetch_address <= next_fetch_address;
    else if (load_new_ip && !mem_access)
        fetch_address <= new_ip;
    else if (!abort_cur && !load_new_ip && (mem_ack || write_second))
        fetch_address <= fetch_address + 1'b1;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        next_fetch_address <= 16'b0;
    else if (load_new_ip)
        next_fetch_address <= new_ip;
end

always_ff @(posedge clk)
    if (mem_ack)
        fetched_high_byte <= mem_data[15:8];

always_ff @(posedge clk)
    next_sequential_fetch <= fetch_address + 1'b1;

endmodule
