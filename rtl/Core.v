module Core(input logic clk,
            input logic reset,
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
            output logic [1:0] data_m_bytesel);

wire [15:0] a_bus;
wire [15:0] b_bus;
wire [15:0] q_bus;
wire [15:0] q_bus_minus_alu;

wire [2:0] reg_rd_sel[2];
wire modrm_complete;
assign reg_rd_sel[0] = modrm_start && ~modrm_complete ? modrm_reg_rd_sel[0] :
    ra_modrm_rm_reg ? rm_regnum : microcode_reg_rd_sel[0];
assign reg_rd_sel[1] = modrm_start && ~modrm_complete ? modrm_reg_rd_sel[1] :
    regnum;

wire [2:0] modrm_reg_rd_sel[2];
wire [2:0] microcode_reg_rd_sel[2];
wire modrm_start;
wire modrm_busy;
wire modrm_uses_bp_as_base;

wire [15:0] reg_rd_val[2];
wire [2:0] microcode_reg_wr_sel;
wire [15:0] reg_wr_val = q_bus;
wire reg_wr_en;
wire [1:0] microcode_seg_wr_sel;
wire [1:0] seg_wr_sel;
assign seg_wr_sel = segment_override ? microcode_seg_wr_sel
    : reg_wr_sel[1:0];
wire [15:0] seg_rd_val;
wire [15:0] seg_wr_val = q_bus;
wire [15:0] cs;
wire fifo_wr_en;
wire modrm_fifo_rd_en;
wire immed_fifo_rd_en;
wire microcode_fifo_rd_en;
wire [1:0] a_sel;
wire [1:0] b_sel;
wire [1:0] q_sel;
wire [`MC_ALUOp_t_BITS-1:0] alu_op;
wire [15:0] alu_out;
wire next_instruction;
wire mar_wr_sel;
wire fifo_rd_en = modrm_fifo_rd_en | immed_fifo_rd_en | microcode_fifo_rd_en;
wire [7:0] fifo_rd_data;
wire [7:0] fifo_wr_data;
wire fifo_empty;
wire fifo_full;
wire fifo_reset;
wire is_8_bit;
wire reg_is_8_bit = modrm_start ? 1'b0 : is_8_bit;
wire [15:0] effective_address;
wire [2:0] regnum;
wire rm_is_reg;
wire [2:0] rm_regnum;
wire [15:0] mar_wr_val;
assign mar_wr_val = mar_wr_sel == MARWrSel_EA ? effective_address : q_bus;

wire ip_inc = fifo_rd_en & ~fifo_empty;
wire ip_wr_en;
wire [15:0] ip_current;

assign a_bus =
    a_sel == ADriver_RA ? reg_rd_val[0] :
    a_sel == ADriver_IP ? ip_current :
    a_sel == ADriver_MAR ? mar : mdr;

assign b_bus =
    b_sel == BDriver_RB ? reg_rd_val[1] :
    b_sel == BDriver_IMMEDIATE ? immediate :
    b_sel == BDriver_SR ? seg_rd_val : 16'b0;

// The Q bus is tapped off before the ALU so that the MAR/MDR can be fed back
// into the A/B busses, but without introducing a combinational loop through
// A/B into the ALU and back out.  Therefore it is not legal to have A/B
// driven from Q and for Q to be driven from the ALU.
assign q_bus_minus_alu =
    q_sel == QDriver_MAR ? mar :
    q_sel == QDriver_MDR ? mdr : 16'b0;
assign q_bus =
    q_sel == QDriver_ALU ? alu_out : q_bus_minus_alu;

wire modrm_immed_start;
wire microcode_immed_start;
wire immed_start = (modrm_immed_start | microcode_immed_start) & ~immed_complete;
wire immed_busy;
wire immed_complete;
wire modrm_immed_is_8bit;
wire immed_is_8bit = modrm_immed_start ? modrm_immed_is_8bit : is_8_bit;
wire [15:0] immediate_reader_immediate;
wire [15:0] immediate = use_microcode_immediate ? microcode_immediate :
    immediate_reader_immediate;

wire microcode_stall = (modrm_busy & ~modrm_complete) |
    (immed_busy & ~immed_complete) |
    loadstore_busy;
wire [15:0] microcode_immediate;
wire use_microcode_immediate;

wire [15:0] mar;
wire [15:0] mdr;
wire write_mdr;
wire write_mar;
wire mem_read;
wire mem_write;
wire ra_modrm_rm_reg;
wire [`MC_RDSelSource_t_BITS-1:0] rd_sel_source;
wire [2:0] reg_wr_sel =
    rd_sel_source == RDSelSource_MODRM_REG ? regnum :
    rd_sel_source == RDSelSource_MODRM_RM_REG ? rm_regnum :
    microcode_reg_wr_sel;

wire loadstore_start = (mem_read | mem_write) & ~loadstore_complete;
wire loadstore_is_store = mem_write;
wire loadstore_complete;
wire loadstore_busy;
wire [1:0] microcode_segment;
wire [1:0] segment = microcode_segment == DS && modrm_uses_bp_as_base ?
    SS : microcode_segment;
wire segment_override;
wire segment_wr_en;
wire [8:0] update_flags;
wire [15:0] flags;
wire [15:0] alu_flags_out;

wire cs_updating = seg_wr_sel == CS && segment_wr_en;
wire prefetch_load_new_ip;
wire [15:0] prefetch_new_ip;

RegisterFile    regfile(.clk(clk),
                        .reset(reset),
                        .is_8_bit(reg_is_8_bit),
                        .rd_sel(reg_rd_sel),
                        .rd_val(reg_rd_val),
                        .wr_sel(reg_wr_sel),
                        .wr_val(reg_wr_val),
                        .wr_en(reg_wr_en));

SegmentRegisterFile segregs(.clk(clk),
                            .reset(reset),
                            .rd_sel(segment),
                            .rd_val(seg_rd_val),
                            .wr_en(segment_wr_en),
                            .wr_sel(seg_wr_sel),
                            .wr_val(seg_wr_val),
                            .cs(cs));

Fifo            #(.data_width(8),
                  .depth(6))
                prefetch_fifo(.clk(clk),
                              .reset(reset | fifo_reset),
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

CSIPSync        ipsync(.clk(clk),
                       .reset(reset),
                       .cs_update(cs_updating),
                       .ip_update(ip_wr_en),
                       .ip_in(ip_current),
                       .new_ip(q_bus),
                       .propagate(next_instruction),
                       .ip_out(prefetch_new_ip),
                       .update_out(prefetch_load_new_ip));

Prefetch        prefetch(.clk(clk),
                         .reset(reset),
                         .new_cs(cs),
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

ImmediateReader immedreader(.clk(clk),
                            .reset(reset),
                            // Control
                            .start(immed_start),
                            .busy(immed_busy),
                            .complete(immed_complete),
                            .is_8bit(immed_is_8bit),
                            // Result
                            .immediate(immediate_reader_immediate),
                            // Fifo read port
                            .fifo_rd_en(immed_fifo_rd_en),
                            .fifo_rd_data(fifo_rd_data),
                            .fifo_empty(fifo_empty));

ModRMDecode     modrmdecode(.clk(clk),
                            .reset(reset),
                            // Control
                            .start(modrm_start),
                            .busy(modrm_busy),
                            .complete(modrm_complete),
                            // Results
                            .effective_address(effective_address),
                            .regnum(regnum),
                            .rm_is_reg(rm_is_reg),
                            .rm_regnum(rm_regnum),
                            .bp_as_base(modrm_uses_bp_as_base),
                            // Registers
                            .reg_sel(modrm_reg_rd_sel),
                            .regs(reg_rd_val),
                            // Fifo Read Port
                            .fifo_rd_en(modrm_fifo_rd_en),
                            .fifo_rd_data(fifo_rd_data),
                            .fifo_empty(fifo_empty),
                            // Immediates
                            .immed_start(modrm_immed_start),
                            .immed_complete(immed_complete),
                            .immed_is_8bit(modrm_immed_is_8bit),
                            .immediate(immediate_reader_immediate));

Flags           flags_reg(.clk(clk),
                          .reset(reset),
                          .flags_in(alu_flags_out),
                          .flags_out(flags),
                          .update_flags(update_flags));

LoadStore       loadstore(.clk(clk),
                          .reset(reset),
                          // MAR
                          .write_mar(write_mar),
                          .segment(seg_rd_val),
                          .mar_in(mar_wr_val),
                          // MDR
                          .mar_out(mar),
                          .mdr_out(mdr),
                          .write_mdr(write_mdr),
                          .mdr_in(q_bus),
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
                          .complete(loadstore_complete));

Microcode       microcode(.clk(clk),
                          .reset(reset),
                          .stall(microcode_stall),
                          .modrm_reg(regnum),
                          .microcode_immediate(microcode_immediate),
                          .use_microcode_immediate(use_microcode_immediate),
                          .rm_is_reg(rm_is_reg),
                          .a_sel(a_sel),
                          .alu_op(alu_op),
                          .b_sel(b_sel),
                          .next_instruction(next_instruction),
                          .read_immed(microcode_immed_start),
                          .load_ip(ip_wr_en),
                          .mar_wr_sel(mar_wr_sel),
                          .mar_write(write_mar),
                          .mdr_write(write_mdr),
                          .mem_read(mem_read),
                          .mem_write(mem_write),
                          .modrm_start(modrm_start),
                          .q_sel(q_sel),
                          .ra_modrm_rm_reg(ra_modrm_rm_reg),
                          .ra_sel(microcode_reg_rd_sel[0]),
                          .rd_sel_source(rd_sel_source),
                          .rd_sel(microcode_reg_wr_sel),
                          .reg_wr_en(reg_wr_en),
                          .segment(microcode_segment),
                          .segment_override(segment_override),
                          .segment_wr_en(segment_wr_en),
                          .sr_wr_sel(microcode_seg_wr_sel),
                          .update_flags(update_flags),
                          .width(is_8_bit),
                          .fifo_rd_en(microcode_fifo_rd_en),
                          .fifo_rd_data(fifo_rd_data),
                          .fifo_empty(fifo_empty));

IP              ip(.clk(clk),
                   .reset(reset),
                   .inc(ip_inc),
                   .wr_en(prefetch_load_new_ip),
                   .wr_val(prefetch_new_ip),
                   .val(ip_current));

ALU             alu(.a(a_bus),
                    .b(b_bus),
                    .out(alu_out),
                    .op(alu_op),
                    .is_8_bit(is_8_bit),
                    .flags_in(flags),
                    .flags_out(alu_flags_out));

`ifdef verilator
// verilator lint_off BLKANDNBLK
int instr_length;
// verilator lint_on BLKANDNBLK

always @(posedge clk)
    if (fifo_rd_en & ~fifo_empty)
        instr_length <= instr_length + 1;

export "DPI-C" function get_and_clear_instr_length;

function int get_and_clear_instr_length;
    get_and_clear_instr_length = instr_length;
    instr_length = 0;
endfunction

`endif

endmodule
