module ModRMTestbench(input logic clk,
                      input logic reset,
                      // Control.
                      input logic start,
                      output logic busy,
                      output logic complete,
                      // Results
                      output logic [15:0] effective_address,
                      output logic [2:0] regnum,
                      output logic rm_is_reg,
                      output logic [2:0] rm_regnum,
                      // Registers.
                      output logic [2:0] reg_sel[2],
                      input logic [15:0] regs[2],
                      // Fifo Read Port.
                      output logic fifo_rd_en,
                      input logic [7:0] fifo_rd_data,
                      input logic fifo_empty);

wire immed_is_8bit;
wire immed_start;
wire immed_complete;
wire immed_fifo_rd_en;
wire modrm_fifo_rd_en;
wire [15:0] immediate;

assign fifo_rd_en = immed_fifo_rd_en | modrm_fifo_rd_en;

ImmediateReader ir(.clk(clk),
                   .reset(reset),
                   .start(immed_start),
                   .complete(immed_complete),
                   .is_8bit(immed_is_8bit),
                   .immediate(immediate),
                   .fifo_rd_en(immed_fifo_rd_en),
                   .fifo_rd_data(fifo_rd_data),
                   .fifo_empty(fifo_empty));

ModRMDecode mrm(.clk(clk),
                .reset(reset),
                .start(start),
                .busy(busy),
                .complete(complete),
                .effective_address(effective_address),
                .regnum(regnum),
                .rm_is_reg(rm_is_reg),
                .rm_regnum(rm_regnum),
                .reg_sel(reg_sel),
                .regs(regs),
                .fifo_rd_en(modrm_fifo_rd_en),
                .fifo_rd_data(fifo_rd_data),
                .fifo_empty(fifo_empty),
                .immed_start(immed_start),
                .immed_complete(immed_complete),
                .immed_is_8bit(immed_is_8bit),
                .immediate(immediate));
endmodule
