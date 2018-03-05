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
module Core(input logic clk,
            input logic reset,
            // Interrupts
            input logic nmi,
            input logic intr,
            input logic [7:0] irq,
            output logic inta,
            // Instruction bus
            output logic [19:1] instr_m_addr,
            input logic [15:0] instr_m_data_in,
            output logic instr_m_access,
            input logic instr_m_ack,
            // Data bus
            output logic [19:1] data_m_addr,
            input logic [15:0] data_m_data_in,
            output logic [15:0] data_m_data_out,
            output logic data_m_access,
            input logic data_m_ack,
            output logic data_m_wr_en,
            output logic [1:0] data_m_bytesel,
            output logic d_io,
            output logic lock,
            // Debug
            output logic debug_stopped,
            input logic debug_seize,
            input logic [7:0] debug_addr,
            input logic debug_run,
            output logic [15:0] debug_val,
            input logic [15:0] debug_wr_val,
            input logic debug_wr_en);

// verilator lint_off UNUSED
Instruction wr_instruction, cur_instruction, next_instruction_value;
// verilator lint_on UNUSED

// Internal busses.
wire [15:0] a_bus =
    a_sel == ADriver_RA ? reg_rd_val[0] :
    a_sel == ADriver_IP ? ip_current :
    a_sel == ADriver_MAR ? mar : mdr;
wire [15:0] b_bus =
    b_sel == BDriver_RB ? reg_rd_val[1] :
    b_sel == BDriver_IMMEDIATE ? immediate :
    b_sel == BDriver_IMMEDIATE2 ? cur_instruction.immediates[1] :
    b_sel == BDriver_SR ? seg_rd_val : tmp_val;

// Register file.
wire [15:0] si, di, bp, bx;
wire reg_is_8_bit = is_8_bit;
wire [2:0] reg_rd_sel[2];
assign reg_rd_sel[0] = ra_modrm_rm_reg ? rm_regnum : microcode_reg_rd_sel[0];
assign reg_rd_sel[1] = rb_cl ? CL : regnum;
wire [2:0] reg_wr_sel =
    rd_sel_source == RDSelSource_MODRM_REG ? regnum :
    rd_sel_source == RDSelSource_MODRM_RM_REG ? rm_regnum :
    microcode_reg_wr_sel;
wire [15:0] reg_wr_val =
    reg_wr_source == RegWrSource_Q ? alu_out[15:0] :
    reg_wr_source == RegWrSource_QUOTIENT ? quotient : remainder;
wire reg_wr_en;
wire [15:0] reg_rd_val[2];
wire rb_cl;
wire [`MC_RDSelSource_t_BITS-1:0] rd_sel_source;

// Segment register file.
wire io_operation;
assign d_io = io_operation;
wire segment_force;
assign seg_wr_sel = segment_force ?
    microcode_segment : reg_wr_sel[1:0];
wire [15:0] seg_rd_val;
wire [15:0] seg_wr_val = alu_out[15:0];
wire [15:0] cs;
wire [1:0] segment;
wire segment_wr_en;

wire decode_immed_start;
wire decode_immed_is_8bit;
wire decode_fifo_rd_en;
wire decode_complete;
wire instruction_empty;
wire instruction_fifo_full;
wire instruction_fifo_nearly_full;

// Prefetch FIFO
wire fifo_wr_en;
wire fifo_rd_en = immed_fifo_rd_en | decode_fifo_rd_en;
wire [7:0] fifo_rd_data;
wire [7:0] fifo_wr_data;
wire fifo_empty;
wire fifo_full;
wire fifo_reset;

// CS:IP Synchronizer
wire cs_updating = seg_wr_sel == CS && segment_wr_en;
wire [15:0] prefetch_cs = cs_updating ? seg_wr_val : cs;
wire ip_wr_en;
wire [15:0] ip_current;
wire prefetch_load_new_ip;
wire [15:0] prefetch_new_ip;

// Immediate Reader
wire immed_start = decode_immed_start;
wire immed_complete;
wire immed_is_8bit = decode_immed_is_8bit;
wire [15:0] immediate_reader_immediate;
wire [15:0] immediate = use_microcode_immediate ? microcode_immediate :
    cur_instruction.immediates[0];
wire immed_fifo_rd_en;

wire instruction_fifo_rd_en;

// ModRM Decoder
wire modrm_clear = reset | do_next_instruction | start_interrupt | fifo_reset;
wire modrm_start;
wire modrm_uses_bp_as_base;
wire [2:0] regnum;
wire rm_is_reg;
wire [2:0] rm_regnum;
wire ra_modrm_rm_reg;

// Flags
wire [15:0] flags;
wire [8:0] update_flags;

// LoadStore
wire [15:0] mar;
wire [15:0] mdr;
wire microcode_write_mdr;
wire write_mdr = microcode_write_mdr | irq_to_mdr;
wire [15:0] mdr_in = microcode_write_mdr ? alu_out[15:0] : {8'b0, irq};
wire microcode_write_mar;
wire write_mar = microcode_write_mar & next_microinstruction;
wire mem_read;
wire mem_write;
wire mar_wr_sel;
wire [15:0] mar_wr_val;
wire loadstore_start = (mem_read | mem_write) & ~loadstore_complete;
wire loadstore_is_store = mem_write;
wire loadstore_complete;
wire loadstore_busy;
assign mar_wr_val = mar_wr_sel == MARWrSel_EA ?
    effective_address : alu_out[15:0];

// ALU
wire [`MC_ALUOp_t_BITS-1:0] alu_op;
wire [31:0] alu_out;
wire [15:0] alu_flags_out;
wire alu_busy;

// Microcode
wire [2:0] microcode_reg_rd_sel[2];
wire [2:0] microcode_reg_wr_sel;
wire [1:0] reg_wr_source;
wire [1:0] seg_wr_sel;
wire [1:0] a_sel;
wire [2:0] b_sel;
wire next_instruction;
wire is_8_bit;
wire [15:0] effective_address;
wire microcode_tmp_wr_en;
wire tmp_wr_en = microcode_tmp_wr_en | (debug_wr_en && debug_stopped);
wire [15:0] tmp_wr_val = debug_stopped && debug_wr_en ? debug_wr_val :
    (tmp_wr_sel == TEMPWrSel_Q_LOW) ?  alu_out[15:0] :
    alu_out[31:16];
wire tmp_wr_sel;
wire [15:0] tmp_val;
wire [15:0] microcode_immediate;
wire use_microcode_immediate;
wire [1:0] microcode_segment;
wire [7:0] opcode;
wire jump_taken;
wire multibit_shift;
wire rb_zero = ~|reg_rd_val[1];
wire nmi_pulse;
wire ext_int_yield;
wire irq_to_mdr;
wire loop_next;
wire loop_done;
wire is_hlt;
wire next_microinstruction;

// Misc control signals
wire debug_set_ip = debug_stopped && ip_wr_en;
wire do_next_instruction = (next_instruction & ~do_stall) | debug_set_ip;
wire do_stall = loadstore_busy | divide_busy | alu_busy;
wire start_interrupt;

// IP
wire do_escape_fault;
wire starting_instruction;
wire ip_rollback = (start_interrupt & ext_int_yield & ~is_hlt) | do_escape_fault;

// Divider
wire [31:0] dividend8 = divide_signed ? {{16{tmp_val[15]}}, tmp_val} : {16'b0, tmp_val};
wire [31:0] dividend = is_8_bit ? dividend8 : {reg_rd_val[0], tmp_val};
wire [15:0] divisor8 = divide_signed ? {{8{mdr[7]}}, mdr[7:0]} : mdr;
wire [15:0] divisor = is_8_bit ? divisor8 : mdr;
wire [15:0] quotient;
wire [15:0] remainder;
wire divide_error;
wire divide_busy;
wire divide = alu_op == ALUOp_DIV || alu_op == ALUOp_IDIV;
wire divide_signed = alu_op == ALUOp_IDIV;
wire divide_complete;
wire do_divide = divide & ~divide_complete;

assign debug_val = tmp_val;

RegisterFile    RegisterFile(.clk(clk),
                             .reset(reset),
                             .si(si),
                             .di(di),
                             .bp(bp),
                             .bx(bx),
                             .is_8_bit(reg_is_8_bit),
                             .rd_sel(reg_rd_sel),
                             .rd_val(reg_rd_val),
                             .wr_sel(reg_wr_sel),
                             .wr_val(reg_wr_val),
                             .wr_en(reg_wr_en));

SegmentOverride SegmentOverride(.clk(clk),
                                .reset(reset),
                                .flush(fifo_reset),
                                .next_instruction(do_next_instruction),
                                .force_segment(segment_force),
                                .bp_is_base(modrm_uses_bp_as_base),
                                .update(starting_instruction),
                                .segment_override(next_instruction_value.has_segment_override),
                                .override_in(next_instruction_value.segment),
                                .microcode_sr_rd_sel(microcode_segment),
                                .sr_rd_sel(segment));

SegmentRegisterFile SegmentRegisterFile(.clk(clk),
                                        .reset(reset),
                                        .rd_sel(segment),
                                        .rd_val(seg_rd_val),
                                        .wr_en(segment_wr_en),
                                        .wr_sel(seg_wr_sel),
                                        .wr_val(seg_wr_val),
                                        .cs(cs));

Fifo            #(.data_width(8),
                  .depth(6))
                PrefetchFifo(.clk(clk),
                             .reset(reset),
                             .flush(fifo_reset),
                             .wr_en(fifo_wr_en),
                             .wr_data(fifo_wr_data),
                             .rd_en(fifo_rd_en),
                             .rd_data(fifo_rd_data),
                             .empty(fifo_empty),
                             .nearly_full(fifo_full),
                             // verilator lint_off PINCONNECTEMPTY
                             .full()
                             // verilator lint_on PINCONNECTEMPTY
                            );

CSIPSync        CSIPSync(.clk(clk),
                         .reset(reset),
                         .cs_update(cs_updating),
                         .ip_update(ip_wr_en),
                         .ip_in(ip_current),
                         .new_ip(alu_out[15:0]),
                         .propagate(do_next_instruction),
                         .ip_out(prefetch_new_ip),
                         .update_out(prefetch_load_new_ip));

TempReg         TempReg(.clk(clk),
                        .reset(reset),
                        .wr_val(tmp_wr_val),
                        .wr_en(tmp_wr_en),
                        .val(tmp_val));

Prefetch        Prefetch(.clk(clk),
                         .reset(reset),
                         .new_cs(prefetch_cs),
                         .new_ip(prefetch_new_ip),
                         .load_new_ip(prefetch_load_new_ip),
                         .fifo_wr_en(fifo_wr_en),
                         .fifo_wr_data(fifo_wr_data),
                         .fifo_reset(fifo_reset),
                         .fifo_full(fifo_full),
                         .mem_access(instr_m_access),
                         .mem_ack(instr_m_ack),
                         .mem_address(instr_m_addr),
                         .mem_data(instr_m_data_in));

ImmediateReader ImmediateReader(.clk(clk),
                                .reset(reset),
                                // Control
                                .start(immed_start),
                                .flush(fifo_reset),
                                // verilator lint_off PINCONNECTEMPTY
                                .busy(),
                                // verilator lint_on PINCONNECTEMPTY
                                .complete(immed_complete),
                                .is_8bit(immed_is_8bit),
                                // Result
                                .immediate(immediate_reader_immediate),
                                // Fifo read port
                                .fifo_rd_en(immed_fifo_rd_en),
                                .fifo_rd_data(fifo_rd_data),
                                .fifo_empty(fifo_empty));

LoopCounter LoopCounter(.clk(clk),
                        .count_in(next_instruction_value.immediates[1][4:0]),
                        .load(instruction_fifo_rd_en),
                        .next(loop_next),
                        .done(loop_done));

Fifo            #(.data_width($bits(Instruction)),
                  .depth(4),
                  .full_threshold(1))
                InstructionFifo(.clk(clk),
                                .reset(reset),
                                .flush(fifo_reset),
                                .wr_en(decode_complete),
                                .wr_data(wr_instruction),
                                .rd_en(instruction_fifo_rd_en),
                                .rd_data(next_instruction_value),
                                .empty(instruction_empty),
                                .nearly_full(instruction_fifo_nearly_full),
                                .full(instruction_fifo_full));

InsnDecoder InsnDecoder(.clk(clk),
                        .reset(reset),
                        .flush(fifo_reset),
                        .stall(instruction_fifo_full),
                        .nearly_full(instruction_fifo_nearly_full),
                        .fifo_rd_en(decode_fifo_rd_en),
                        .fifo_rd_data(fifo_rd_data),
                        .fifo_empty(fifo_empty),
                        .instruction(wr_instruction),
                        .complete(decode_complete),
                        .immed_start(decode_immed_start),
                        .immed_complete(immed_complete),
                        .immed_is_8bit(decode_immed_is_8bit),
                        .immediate(immediate_reader_immediate));

ModRMDecode     ModRMDecode(.clk(clk),
                            .reset(reset),
                            .si(si),
                            .di(di),
                            .bp(bp),
                            .bx(bx),
                            // Control
                            .start(modrm_start),
                            .clear(modrm_clear),
                            .modrm(next_instruction_value.mod_rm),
                            .displacement(next_instruction_value.displacement),
                            // Results
                            .effective_address(effective_address),
                            .regnum(regnum),
                            .rm_is_reg(rm_is_reg),
                            .rm_regnum(rm_regnum),
                            .bp_as_base(modrm_uses_bp_as_base));

Flags           Flags(.clk(clk),
                      .reset(reset),
                      .flags_in(alu_flags_out),
                      .flags_out(flags),
                      .update_flags(update_flags));

JumpTest        JumpTest(.opcode(opcode),
                         .flags(flags),
                         .taken(jump_taken));

LoadStore       LoadStore(.clk(clk),
                          .reset(reset),
                          // MAR
                          .write_mar(write_mar),
                          .segment(seg_rd_val),
                          .mar_in(mar_wr_val),
                          // MDR
                          .mar_out(mar),
                          .mdr_out(mdr),
                          .write_mdr(write_mdr),
                          .mdr_in(mdr_in),
                          // Memory bus
                          .m_addr(data_m_addr),
                          .m_data_in(data_m_data_in),
                          .m_data_out(data_m_data_out),
                          .m_access(data_m_access),
                          .m_ack(data_m_ack),
                          .m_wr_en(data_m_wr_en),
                          .m_bytesel(data_m_bytesel),
                          // Control
                          .start(loadstore_start),
                          .is_8bit(is_8_bit),
                          .wr_en(loadstore_is_store),
                          .busy(loadstore_busy),
                          .complete(loadstore_complete),
                          .io(io_operation));

PosedgeToPulse PosedgeToPulse(.d(nmi),
                              .q(nmi_pulse),
                              .*);

Microcode       Microcode(.clk(clk),
                          .reset(reset),
                          .nmi_pulse(nmi_pulse),
                          .intr(intr),
                          .inta(inta),
                          .irq_to_mdr(irq_to_mdr),
                          .start_interrupt(start_interrupt),
                          .do_escape_fault(do_escape_fault),
                          .starting_instruction(starting_instruction),
                          .stall(do_stall),
                          .divide_error(divide_error),
                          .modrm_reg(regnum),
                          .int_enabled(flags[IF_IDX]),
                          .zf(flags[ZF_IDX]),
                          .tf(flags[TF_IDX]),
                          .microcode_immediate(microcode_immediate),
                          .use_microcode_immediate(use_microcode_immediate),
                          .opcode(opcode),
                          .jump_taken(jump_taken),
                          .rb_zero(rb_zero),
                          .lock(lock),
                          .multibit_shift(multibit_shift),
                          .rm_is_reg(rm_is_reg),
                          .a_sel(a_sel),
                          .alu_op(alu_op),
                          .b_sel(b_sel),
                          .ext_int_yield(ext_int_yield),
                          .io(io_operation),
                          .next_instruction(next_instruction),
                          .load_ip(ip_wr_en),
                          .mar_wr_sel(mar_wr_sel),
                          .mar_write(microcode_write_mar),
                          .mdr_write(microcode_write_mdr),
                          .mem_read(mem_read),
                          .mem_write(mem_write),
                          .modrm_start(modrm_start),
                          .ra_modrm_rm_reg(ra_modrm_rm_reg),
                          .ra_sel(microcode_reg_rd_sel[0]),
                          .rb_cl(rb_cl),
                          .rd_sel_source(rd_sel_source),
                          .rd_sel(microcode_reg_wr_sel),
                          .reg_wr_en(reg_wr_en),
                          .reg_wr_source(reg_wr_source),
                          .segment(microcode_segment),
                          .segment_force(segment_force),
                          .segment_wr_en(segment_wr_en),
                          .tmp_wr_en(microcode_tmp_wr_en),
                          .tmp_wr_sel(tmp_wr_sel),
                          .update_flags(update_flags),
                          .width(is_8_bit),
                          .fifo_rd_en(instruction_fifo_rd_en),
                          .cur_instruction(cur_instruction),
                          .next_instruction_value(next_instruction_value),
                          .fifo_empty(instruction_empty),
                          .fifo_resetting(fifo_reset),
                          .loop_next(loop_next),
                          .loop_done(loop_done),
                          .is_hlt(is_hlt),
                          .next_microinstruction(next_microinstruction),
                          // Debug
                          .debug_stopped(debug_stopped),
                          .debug_seize(debug_seize),
                          .debug_addr(debug_addr),
                          .debug_run(debug_run));

IP              IP(.clk(clk),
                   .reset(reset),
                   .inc(next_instruction_value.length),
                   .start_instruction(instruction_fifo_rd_en),
                   .next_instruction(do_next_instruction),
                   .rollback(ip_rollback),
                   .wr_en(prefetch_load_new_ip),
                   .wr_val(prefetch_new_ip),
                   .val(ip_current));

ALU             ALU(.a(a_bus),
                    .b(b_bus),
                    .out(alu_out),
                    .op(alu_op),
                    .is_8_bit(is_8_bit),
                    .flags_in(flags),
                    .flags_out(alu_flags_out),
                    .multibit_shift(multibit_shift),
                    .shift_count(tmp_val[4:0]),
                    .busy(alu_busy));

Divider         Divider(.clk(clk),
                        .reset(reset),
                        .start(do_divide),
                        .is_8_bit(is_8_bit),
                        .is_signed(divide_signed),
                        .busy(divide_busy),
                        .complete(divide_complete),
                        .error(divide_error),
                        .dividend(dividend),
                        .divisor(divisor),
                        .quotient(quotient),
                        .remainder(remainder));

`ifdef verilator
// verilator lint_off BLKANDNBLK
int instr_length;
// verilator lint_on BLKANDNBLK

always @(posedge clk)
    if (instruction_fifo_rd_en)
        instr_length <= {28'b0, next_instruction_value.length};

export "DPI-C" function get_and_clear_instr_length;

function int get_and_clear_instr_length;
    get_and_clear_instr_length = instr_length;
    instr_length = 0;
endfunction

`endif

endmodule
