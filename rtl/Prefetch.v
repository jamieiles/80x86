/*
 * Prefetcher
 *
 * This module is responsible for fetching bytes from the memory and pushing
 * them into the instruction stream FIFO.  The CS is stored outside of the
 * Prefetch unit and passed in to be combined with the internal IP which is
 * wired out.  This means that IP wrapping works correctly, and can be updated
 * under external control.  The fetching can be stalled when servicing
 * a branch, updating the IP will flush the FIFO.
 */
module Prefetch(input logic clk,
                input logic reset,
                /* Address control. */
                input logic [15:0] cs,
                input logic [15:0] new_ip,
                input logic load_new_ip,
                /* FIFO write port. */
                output logic fifo_wr_en,
                output logic [7:0] fifo_wr_data,
                output logic fifo_reset,
                input logic fifo_full,
                /* Memory port. */
                output logic mem_access,
                input logic mem_ack,
                output logic [19:0] mem_address,
                input logic [15:0] mem_data);

reg [15:0] fetch_address;
reg [7:0] fetched_high_byte;
reg write_second;
wire should_write_second_byte = mem_ack && !abort_cur && !fetch_address[0];
reg abort_cur;

/* verilator lint_off UNUSED */
wire [15:0] next_address = mem_ack && !abort_cur ? fetch_address + 1'b1 : fetch_address;
/* verilator lint_on UNUSED */

assign mem_address = {cs, 4'b0} + {4'b0, next_address[15:1], 1'b0};
assign mem_access = !reset && !fifo_full && !mem_ack;

assign fifo_wr_en = !abort_cur && !load_new_ip && (mem_ack || write_second);
assign fifo_reset = load_new_ip;
assign fifo_wr_data = mem_ack ?
    (fetch_address[0] ? mem_data[15:8] : mem_data[7:0]) : fetched_high_byte;

always_ff @(posedge clk or posedge reset) begin
    if (reset || (abort_cur && mem_ack))
        abort_cur <= 1'b0;
    else if (mem_access && load_new_ip)
        abort_cur <= 1'b1;
end

always_ff @(posedge clk or posedge reset)
    write_second <= reset ? 1'b0 : should_write_second_byte;

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        fetch_address <= 16'b0;
    else if (load_new_ip)
        fetch_address <= new_ip;
    else if (!abort_cur && (mem_ack || write_second))
        fetch_address <= fetch_address + 1'b1;
end

always_ff @(posedge clk)
    if (mem_ack)
        fetched_high_byte <= mem_data[15:8];

endmodule
