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

// PIC-"lite"
//
// Only supports a single PIC, edge triggered interrupts and non-specific or
// specific EOI.  No rotating priorities etc.
`default_nettype none
module PIC(input logic clk,
           input logic reset,
           input logic cs,
           input logic [15:0] data_m_data_in,
           output logic [15:0] data_m_data_out,
           input logic [1:0] data_m_bytesel,
           input logic data_m_wr_en,
           input logic data_m_access,
           output logic data_m_ack,
           input logic [7:0] intr_in,
           output logic [7:0] irq,
           output logic intr,
           input logic inta);

enum bit [2:0] {
    INIT_STATE_ICW1,
    INIT_STATE_ICW2,
    INIT_STATE_ICW3,
    INIT_STATE_ICW4,
    INIT_STATE_IDLE
} init_state, next_init_state;

enum bit [1:0] {
    REG_LATCH_NONE,
    REG_LATCH_IRR,
    REG_LATCH_ISR
} reg_latch;

reg [7:3] vector_address;
reg [7:0] mask, isr, irr;
reg [2:0] delivered;

reg [7:0] intr_last;
wire [7:0] intr_edges = (intr_in ^ intr_last) & intr_in;

wire [7:0] command_read_val = reg_latch == REG_LATCH_IRR ? irr :
                              reg_latch == REG_LATCH_ISR ? isr : 8'b0;

wire access_command = cs & data_m_access & data_m_bytesel[0];
wire access_data = cs & data_m_access & data_m_bytesel[1];

always_ff @(posedge clk)
    intr_last <= intr_in;

always_comb begin
    case (init_state)
    INIT_STATE_ICW1: next_init_state =
        access_command & data_m_wr_en & data_m_data_in[4] ?
            INIT_STATE_ICW2 : INIT_STATE_ICW1;
    INIT_STATE_ICW2: next_init_state = access_data & data_m_wr_en ?
        INIT_STATE_ICW4 : INIT_STATE_ICW2;
    INIT_STATE_ICW4: next_init_state = access_data & data_m_wr_en ?
        INIT_STATE_IDLE : INIT_STATE_ICW4;
    default: next_init_state = init_state;
    endcase

    if (reset)
        next_init_state = INIT_STATE_ICW1;
end

always_ff @(posedge clk)
    init_state <= next_init_state;

always_ff @(posedge clk)
    if (cs && data_m_access && !data_m_wr_en)
        data_m_data_out <= {mask, command_read_val};
    else
        data_m_data_out <= 16'b0;

always_ff @(posedge clk)
    data_m_ack <= cs & data_m_access;

always_ff @(posedge reset or posedge clk)
    if (reset)
        mask <= 8'b0;
    else if (init_state == INIT_STATE_IDLE && access_data && data_m_wr_en)
        mask <= data_m_data_in[15:8];

always_ff @(posedge reset or posedge clk)
    if (reset)
        vector_address <= 5'b0;
    else if (access_data && data_m_wr_en && init_state == INIT_STATE_ICW2)
        vector_address <= data_m_data_in[15:11];

always_ff @(posedge reset or posedge clk)
    if (reset)
        reg_latch <= REG_LATCH_NONE;
    else if (init_state == INIT_STATE_IDLE && access_command && data_m_wr_en &&
             data_m_data_in[3])
        reg_latch <= data_m_data_in[1:0] == 2'b10 ? REG_LATCH_IRR :
                     data_m_data_in[1:0] == 2'b11 ? REG_LATCH_ISR :
                     REG_LATCH_NONE;

always_ff @(posedge reset or posedge clk)
    if (reset)
        irr <= 8'b0;
    else begin
        if (inta)
            irr[delivered] <= 1'b0;
        else
            irr <= irr | (~mask & intr_edges);
    end

always_ff @(posedge reset or posedge clk)
    if (reset)
        isr <= 8'b0;
    else begin
        if (access_command && data_m_wr_en &&
            data_m_data_in[4:3] == 2'b00) begin
            if (data_m_data_in[7:5] == 3'b001) begin
                for (int i = 0; i < 8; i += 1) begin
                    if (isr[i]) begin
                        isr[i] <= 1'b0;
                        break;
                    end
                end
            end else if (data_m_data_in[7:5] == 3'b011) begin
                isr[data_m_data_in[2:0]] <= 1'b0;
            end
        end
        if (inta)
            isr[delivered] <= 1'b1;
    end

always_comb begin
    intr = 1'b0;
    irq = 8'b0;

    for (logic [3:0] i = 4'b0; i < 4'd8; i += 1'b1) begin
        if (inta || isr[i[2:0]])
            break;
        if (irr[i[2:0]]) begin
            intr = 1'b1;
            irq = {vector_address, i[2:0]};
            break;
        end
    end
end

always_ff @(posedge clk)
    delivered <= irq[2:0];

endmodule
