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
module JTAGBridge(input logic tck,
                  input logic cpu_clk,
                  input logic reset,
                  input logic [1:0] ir,
                  output logic tdo,
                  input logic tdi,
                  input logic sdr,
                  input logic cdr,
                  input logic udr,
                  input logic debug_stopped,
                  output logic debug_seize,
                  output logic debug_reset,
                  output logic debug_run,
                  output logic [7:0] debug_addr,
                  output logic [15:0] debug_wr_val,
                  input logic [15:0] debug_val,
                  output logic debug_wr_en);

typedef enum bit [1:0] {
    IDCODE,
    STATUS_CONTROL,
    VALUE,
    RUN_PROCEDURE
} DEBUG_REG_t;

reg bypass;
reg [31:0] idcode_reg;
reg status_control_reg_wr_en;
reg [15:0] status_control_reg;
reg [16:0] value_reg;
wire value_reg_wr_en = value_reg[16];
reg [15:0] shadow_value_reg;
reg [7:0] run_procedure_reg;

reg jtag_seize = 1'b0;
reg jtag_reset = 1'b0;
reg jtag_update_value = 1'b0;
reg jtag_run_prodedure = 1'b0;
reg jtag_debug_busy = 1'b0;
wire [15:0] shadow_value_wr_val;
wire jtag_update_debug_val;
wire jtag_debug_stopped;

reg debug_running = 1'b0;
reg debug_complete_load = 1'b0;
wire debug_val_ready;

assign tdo = sdr && ir == IDCODE ? idcode_reg[0] :
             sdr && ir == STATUS_CONTROL ? status_control_reg[0] :
             sdr && ir == VALUE ? value_reg[0] :
             sdr && ir == RUN_PROCEDURE ? 1'b0 : bypass;

BitSync StoppedSync(.clk(tck),
                    .d(debug_stopped),
                    .q(jtag_debug_stopped));

BitSync SeizeSync(.clk(cpu_clk),
                  .d(jtag_seize),
                  .q(debug_seize));

BitSync JTAGResetSync(.clk(cpu_clk),
                      .d(jtag_reset),
                      .q(debug_reset));

// Debug write value, JTAG -> CPU
MCP     #(.width(16),
          .reset_val(16'b0))
        DebugValueMCP(.reset(reset),
                      .clk_a(tck),
                      .a_ready(),
                      .a_send(jtag_update_value),
                      .a_datain(value_reg[15:0]),
                      .clk_b(cpu_clk),
                      .b_data(debug_wr_val),
                      .b_load(debug_wr_en));

// Debug read value, CPU -> JTAG
MCP     #(.width(16),
          .reset_val(16'b0))
        DebugReadValueMCP(.reset(reset),
                          .clk_a(cpu_clk),
                          .a_ready(debug_val_ready),
                          .a_send(debug_complete_load),
                          .a_datain(debug_val),
                          .clk_b(tck),
                          .b_data(shadow_value_wr_val),
                          .b_load(jtag_update_debug_val));

// Debug procedure run, JTAG -> CPU
MCP     #(.width(8),
          .reset_val(8'b0))
        DebugProcMCP(.reset(reset),
                     .clk_a(tck),
                     .a_ready(),
                     .a_send(jtag_run_prodedure),
                     .a_datain(run_procedure_reg),
                     .clk_b(cpu_clk),
                     .b_data(debug_addr),
                     .b_load(debug_run));

// Generate debug value load signals from CPU into JTAG domain
always_ff @(posedge cpu_clk or posedge reset)
    if (reset) begin
        debug_running <= 1'b0;
    end else begin
        if (debug_run)
            debug_running <= 1'b1;
        else if (debug_running && debug_stopped && debug_val_ready)
            debug_running <= 1'b0;
    end

always_ff @(posedge cpu_clk)
    debug_complete_load <= debug_running && debug_stopped && debug_val_ready;

always_ff @(posedge tck)
    if (ir == IDCODE) begin
        if (cdr)
            idcode_reg <= 32'h53454c49;
        else if (sdr)
            idcode_reg <= {tdi, idcode_reg[31:1]};
    end

always_ff @(posedge tck)
    if (ir == STATUS_CONTROL) begin
        if (cdr)
            status_control_reg <= {14'b0, debug_reset, ~jtag_debug_stopped};
        else if (sdr)
            {status_control_reg_wr_en, status_control_reg} <=
                {tdi, status_control_reg_wr_en, status_control_reg[15:1]};
        else if (udr)
            status_control_reg_wr_en <= 1'b0;
    end

always_ff @(posedge tck)
    if (ir == VALUE) begin
        if (cdr)
            value_reg <= {~jtag_debug_busy, shadow_value_reg};
        else if (sdr)
            value_reg <= {tdi, value_reg[16:1]};
        else if (udr)
            value_reg[16] <= 1'b0;
    end

always_ff @(posedge tck)
    if (ir == RUN_PROCEDURE && sdr)
        run_procedure_reg <= {tdi, run_procedure_reg[7:1]};

always_ff @(posedge tck)
    bypass <= tdi;

always_ff @(posedge tck)
    if (jtag_update_debug_val)
        shadow_value_reg <= shadow_value_wr_val;

always_ff @(posedge tck)
    if (jtag_update_debug_val)
        jtag_debug_busy <= 1'b0;
    else
        jtag_debug_busy <= (ir == RUN_PROCEDURE) && udr;

always_ff @(posedge tck)
    if (ir == STATUS_CONTROL && udr && status_control_reg_wr_en) begin
        jtag_seize <= ~status_control_reg[0];
        jtag_reset <= status_control_reg[1];
    end

always_ff @(posedge tck)
    jtag_run_prodedure <= (ir == RUN_PROCEDURE) && udr;

always_ff @(posedge tck)
    jtag_update_value <= (ir == VALUE) && udr && value_reg_wr_en;

endmodule
