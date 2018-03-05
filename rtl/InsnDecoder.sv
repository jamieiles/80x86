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

module InsnDecoder(input logic clk,
                   input logic reset,
                   input logic flush,
                   input logic stall,
                   input logic nearly_full,
                   output logic fifo_rd_en,
                   input logic [7:0] fifo_rd_data,
                   input logic fifo_empty,
                   output Instruction instruction,
                   output logic complete,
                   output logic immed_start,
                   input logic immed_complete,
                   output logic immed_is_8bit,
                   input logic [15:0] immediate);

typedef enum bit[2:0] {
    STATE_OPCODE,
    STATE_MODRM,
    STATE_DISPLACEMENT,
    STATE_IMMED1,
    STATE_IMMED2,
    STATE_WAIT_SPACE
} state_t;

state_t state, next_state;

logic insn_complete;
assign fifo_rd_en = ~reset & ~fifo_empty & ~immed_start & state != STATE_WAIT_SPACE;

function logic has_displacement;
    // verilator lint_off UNUSED
    input logic [7:0] mbyte;
    // verilator lint_on UNUSED

    has_displacement = (mbyte[2:0] == 3'b110 && mbyte[7:6] == 2'b00) || ^mbyte[7:6];
endfunction

function logic is_prefix;
    input logic [7:0] b;

    case (b)
    8'h26, 8'h2e, 8'h36, 8'h3e, 8'hf2, 8'hf3, 8'hf0: is_prefix = 1'b1;
    default: is_prefix = 1'b0;
    endcase
endfunction

function state_t next_instr;
    next_instr = (nearly_full || stall) && insn_complete ? STATE_WAIT_SPACE : STATE_OPCODE;
endfunction

always_comb begin
    insn_complete = 1'b0;

    case (state)
    STATE_OPCODE: begin
        insn_complete = (!fifo_empty && !is_prefix(fifo_rd_data) &&
            !insn_has_modrm(fifo_rd_data) &&
            insn_immed_count(fifo_rd_data, 3'b0) == 2'b00) ||
            instruction.invalid;
        next_state = fifo_empty ? STATE_OPCODE :
            is_prefix(fifo_rd_data) ? STATE_OPCODE :
            insn_has_modrm(fifo_rd_data) ? STATE_MODRM :
            insn_immed_count(fifo_rd_data, 3'b0) != 2'b00 ? STATE_IMMED1 :
            insn_complete ? next_instr() : STATE_OPCODE;
    end
    STATE_MODRM: begin
        insn_complete = !fifo_empty && !has_displacement(fifo_rd_data) &&
            insn_immed_count(instruction.opcode, fifo_rd_data[5:3]) == 2'b00;
        next_state = fifo_empty ? STATE_MODRM :
            has_displacement(fifo_rd_data) ? STATE_DISPLACEMENT :
            insn_immed_count(instruction.opcode, fifo_rd_data[5:3]) != 2'b00 ?
                STATE_IMMED1 : next_instr();
    end
    STATE_DISPLACEMENT: begin
        insn_complete = immed_complete &&
            insn_immed_count(instruction.opcode, instruction.mod_rm[5:3]) == 2'b0;
        next_state = immed_complete ?
            (insn_immed_count(instruction.opcode, instruction.mod_rm[5:3]) != 2'b00 ?
                STATE_IMMED1 : next_instr()) : STATE_DISPLACEMENT;
    end
    STATE_IMMED1: begin
        insn_complete = insn_immed_count(instruction.opcode,
                                         instruction.mod_rm[5:3]) == 2'b01 &&
            immed_complete;
        next_state = immed_complete ?
            (insn_immed_count(instruction.opcode, instruction.mod_rm[5:3]) == 2'd2 ?
                STATE_IMMED2 : next_instr()) :
            STATE_IMMED1;
    end
    STATE_IMMED2: begin
        insn_complete = immed_complete;
        next_state = immed_complete ? next_instr(): STATE_IMMED2;
    end
    STATE_WAIT_SPACE: begin
        next_state = stall ? STATE_WAIT_SPACE : STATE_OPCODE;
    end
    default: next_state = STATE_OPCODE;
    endcase

    if (reset || flush)
        next_state = STATE_OPCODE;
end

task add_segment_override;
    input logic [1:0] seg;
begin
    instruction.has_segment_override <= 1'b1;
    instruction.segment <= SR_t'(seg);
end
endtask

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        complete <= 1'b0;
        instruction <= Instruction'(1'b0);
        immed_start <= 1'b0;
    end else if (flush) begin
        complete <= 1'b0;
        instruction <= Instruction'(1'b0);
        immed_start <= 1'b0;
    end else begin
        if (complete && !stall) begin
            instruction <= Instruction'(1'b0);
            complete <= 1'b0;
        end

        if (insn_complete)
            complete <= 1'b1;

        case (state)
        STATE_OPCODE: begin
            if (!fifo_empty) begin
                case (fifo_rd_data)
                8'hf0: instruction.lock <= 1'b1;
                8'hf2: instruction.rep <= REP_PREFIX_NE;
                8'hf3: instruction.rep <= REP_PREFIX_E;
                8'h26: add_segment_override(ES);
                8'h2e: add_segment_override(CS);
                8'h36: add_segment_override(SS);
                8'h3e: add_segment_override(DS);
                default: begin
                    instruction.opcode <= fifo_rd_data;
                    instruction.has_modrm <= insn_has_modrm(fifo_rd_data);
                end
                endcase

                if (complete)
                    instruction.length <= 4'b1;
                else if (!fifo_empty)
                    {instruction.invalid, instruction.length} <=
                        {instruction.invalid, instruction.length} + 5'b1;

                if (next_state == STATE_IMMED1) begin
                    immed_start <= 1'b1;
                    immed_is_8bit <= insn_immed_is_8bit(fifo_rd_data,
                                                        3'b0, 1'b0);
                end
            end
        end
        STATE_MODRM: begin
            if (!fifo_empty) begin
                {instruction.invalid, instruction.length} <=
                    {instruction.invalid, instruction.length} + 5'b1;
                instruction.mod_rm <= fifo_rd_data;
            end
            if (next_state == STATE_DISPLACEMENT) begin
                immed_start <= 1'b1;
                immed_is_8bit <= fifo_rd_data[7:6] == 2'b01;
            end else if (next_state == STATE_IMMED1) begin
                immed_start <= 1'b1;
                immed_is_8bit <= insn_immed_is_8bit(instruction.opcode,
                                                    fifo_rd_data[5:3], 1'b0);
            end
        end
        STATE_DISPLACEMENT: begin
            if (immed_complete) begin
                instruction.displacement <= immediate;
                {instruction.invalid, instruction.length} <=
                    {instruction.invalid, instruction.length} +
                    (immed_is_8bit ? 5'd1 : 5'd2);
                immed_start <= 1'b0;
            end
            if (next_state == STATE_IMMED1) begin
                immed_start <= 1'b1;
                immed_is_8bit <= insn_immed_is_8bit(instruction.opcode,
                                                    instruction.mod_rm[5:3], 1'b0);
            end
        end
        STATE_IMMED1: begin
            if (immed_complete) begin
                instruction.immediates[0] <= immediate;
                {instruction.invalid, instruction.length} <=
                    {instruction.invalid, instruction.length} +
                    (immed_is_8bit ? 5'd1 : 5'd2);
                immed_start <= 1'b0;
            end
            if (next_state == STATE_IMMED2) begin
                immed_start <= 1'b1;
                immed_is_8bit <= insn_immed_is_8bit(instruction.opcode,
                                                    instruction.mod_rm[5:3], 1'b1);
            end
        end
        STATE_IMMED2: begin
            if (immed_complete) begin
                instruction.immediates[1] <= immediate;
                {instruction.invalid, instruction.length} <=
                    {instruction.invalid, instruction.length} +
                    (immed_is_8bit ? 5'd1 : 5'd2);
                immed_start <= 1'b0;
            end
        end
        default: ;
        endcase
    end
end

always_ff @(posedge clk)
    state <= next_state;

endmodule
