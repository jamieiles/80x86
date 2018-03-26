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

/*
 * Simple SDR SDRAM controller for 4Mx16x4 devices, e.g. ISSI IS45S16160G
 * (32MB) or 64MB variants.
 */
`default_nettype none
module SDRAMController #(parameter size = 32 * 1024 * 1024,
                         parameter clkf = 25000000)
                        (input logic clk,
                         input logic reset,
                         input logic data_m_access,
                         input logic cs,
                         /* Host interface. */
                         input logic [25:1] h_addr,
                         input logic [15:0] h_wdata,
                         output logic [15:0] h_rdata /* synthesis ALTERA_ATTRIBUTE = "FAST_INPUT_REGISTER=ON" */,
                         input logic h_wr_en,
                         input logic [1:0] h_bytesel,
                         output logic h_compl,
                         output logic h_config_done,
                         /* SDRAM signals. */
                         output logic s_ras_n,
                         output logic s_cas_n,
                         output logic s_wr_en,
                         output logic [1:0] s_bytesel,
                         output logic [12:0] s_addr,
                         output logic s_cs_n,
                         output logic s_clken,
                         inout [15:0] s_data,
                         output logic [1:0] s_banksel);

localparam ns_per_clk   = (1000000000 / clkf);
localparam tReset       = 100000 / ns_per_clk;
localparam tRC          = 8;
localparam tRP          = 2;
localparam tMRD         = 2;
localparam tRCD         = 2;
localparam cas          = 2;
/*
 * From idle, what is the longest path to get back to idle (excluding
 * autorefresh)?  We need to know this to make sure that we issue the
 * autorefresh command often enough.
 *
 * tRef of 64ms for normal temperatures (< 85C).
 *
 * Need to refresh 8192 times every tRef.
 */
localparam tRef           = ((64 * 1000000) / ns_per_clk) / 8192;
localparam max_cmd_period = tRCD + tRP + 1;
localparam refresh_counter_width = $clog2(tRef);

/* Command truth table: CS  RAS  CAS  WE. */
localparam CMD_NOP        = 4'b0111;
localparam CMD_BST        = 4'b0110;
localparam CMD_READ       = 4'b0101;
localparam CMD_WRITE      = 4'b0100;
localparam CMD_ACT        = 4'b0011;
localparam CMD_PRE        = 4'b0010;
localparam CMD_REF        = 4'b0001;
localparam CMD_MRS        = 4'b0000;

localparam STATE_RESET      = 4'b0000;
localparam STATE_RESET_PCH  = 4'b0001;
localparam STATE_RESET_REF1 = 4'b0011;
localparam STATE_RESET_REF2 = 4'b0010;
localparam STATE_MRS        = 4'b0110;
localparam STATE_IDLE       = 4'b0111;
localparam STATE_ACT        = 4'b0101;
localparam STATE_READ       = 4'b1101;
localparam STATE_WRITE      = 4'b1001;
localparam STATE_PCH        = 4'b1000;
localparam STATE_AUTOREF    = 4'b1010;

reg [3:0] state;
reg [3:0] next_state;

reg [3:0] cmd;

reg [15:0] outdata;
reg [1:0] outbytesel;

reg oe;

assign s_cs_n           = cmd[3];
assign s_ras_n          = cmd[2];
assign s_cas_n          = cmd[1];
assign s_wr_en          = cmd[0];
assign s_data           = oe ? outdata : {16{1'bz}};
assign s_bytesel        = outbytesel;
assign s_clken          = 1'b1;

wire [1:0] h_banksel;
wire [12:0] h_rowsel;
wire [12:0] col;

reg [12:0] open_row[0:3];
reg [3:0] row_is_open;

generate
if (size == 32 * 1024 * 1024) begin
    assign h_banksel            = h_addr[11:10];
    assign h_rowsel             = h_addr[24:12];
    assign col                  = {4'b0, h_addr[9:1]};
end else if (size == 64 * 1024 * 1024) begin
    assign h_banksel            = h_addr[12:11];
    assign h_rowsel             = h_addr[25:13];
    assign col                  = {3'b0, h_addr[10:1]};
end
endgenerate

/*
 * State machine counter.  Counts every cycle, resets on change of state
 * - once we reach one of the timing parameters we can transition again.  On
 * count 0 we emit the command, after that it's NOP's all the way.
 */
localparam timec_width = $clog2(tReset);
wire [timec_width - 1:0] timec;

Counter         #(.count_width(timec_width),
                  .count_max(tReset))
                TimecCounter(.clk(clk),
                             .count(timec),
                             .reset(state != next_state | reset));

/*
 * Make sure that we refresh the correct number of times per refresh period
 * and have sufficient time to complete any transaction in progress.
 */
wire [refresh_counter_width - 1:0] refresh_count;
wire autorefresh_counter_clr = state == STATE_AUTOREF && timec == tRC - 1;

Counter         #(.count_width(refresh_counter_width),
                  .count_max(tRef - max_cmd_period))
                RefreshCounter(.clk(clk),
                               .count(refresh_count),
                               .reset(autorefresh_counter_clr | reset));

wire autorefresh_pending = refresh_count == tRef[refresh_counter_width - 1:0] - max_cmd_period;

always_comb begin
    next_state = state;
    case (state)
    STATE_RESET: begin
        if (timec == tReset[timec_width - 1:0] - 1)
            next_state = STATE_RESET_PCH;
    end
    STATE_RESET_PCH: begin
        if (timec == tRP - 1)
            next_state = STATE_RESET_REF1;
    end
    STATE_RESET_REF1: begin
        if (timec == tRC - 1)
            next_state = STATE_RESET_REF2;
    end
    STATE_RESET_REF2: begin
        if (timec == tRC - 1)
            next_state = STATE_MRS;
    end
    STATE_MRS: begin
        if (timec == tMRD - 1)
            next_state = STATE_IDLE;
    end
    STATE_IDLE: begin
        /*
         * If we have a refresh pending then make sure we handle that
         * first!
         */
        if (!h_compl && autorefresh_pending)
            next_state = STATE_PCH;
        else if (!h_compl && cs && data_m_access) begin
            if (row_is_open[h_banksel] && open_row[h_banksel] == h_rowsel)
                next_state = h_wr_en ? STATE_WRITE : STATE_READ;
            else if (row_is_open[h_banksel] && open_row[h_banksel] != h_rowsel)
                next_state = STATE_PCH;
            else
                next_state = STATE_ACT;
        end
    end
    STATE_ACT: begin
        if (timec == tRCD - 1)
            next_state = h_wr_en ? STATE_WRITE : STATE_READ;
    end
    STATE_WRITE: begin
        if (timec == tRP)
            next_state = STATE_IDLE;
    end
    STATE_READ: begin
        if (timec == cas)
            next_state = STATE_IDLE;
    end
    STATE_PCH: begin
        if (timec == tRP - 1)
            next_state = s_addr[10] ? STATE_AUTOREF : STATE_IDLE;
    end
    STATE_AUTOREF: begin
        if (timec == tRC - 1)
            next_state = STATE_IDLE;
    end
    default: begin
        next_state = STATE_IDLE;
    end
    endcase

    if (reset)
        next_state = STATE_RESET;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        cmd <= CMD_NOP;
        s_addr <= 13'b0;
        s_banksel <= 2'b00;
        oe <= 1'b0;
    end else begin
        cmd <= CMD_NOP;
        if (state != next_state) begin
            case (next_state)
            STATE_IDLE: oe <= 1'b0;
            STATE_RESET_PCH: begin
                cmd <= CMD_PRE;
                s_addr <= 13'b10000000000;
                s_banksel <= 2'b11;
            end
            STATE_RESET_REF1: begin
                cmd <= CMD_REF;
            end
            STATE_RESET_REF2: begin
                cmd <= CMD_REF;
            end
            STATE_MRS: begin
                cmd <= CMD_MRS;
                s_banksel <= 2'b00;
                /* Burst length 2, CAS 2. */
                s_addr <= 13'b1000100000;
            end
            STATE_ACT: begin
                cmd <= CMD_ACT;
                s_banksel <= h_banksel;
                s_addr <= h_rowsel;
            end
            STATE_WRITE: begin
                cmd <= CMD_WRITE;
                s_addr <= col;
                s_banksel <= h_banksel;
                oe <= 1'b1;
            end
            STATE_READ: begin
                cmd <= CMD_READ;
                s_addr <= col;
                s_banksel <= h_banksel;
            end
            STATE_AUTOREF: begin
                cmd <= CMD_REF;
            end
            STATE_PCH: begin
                cmd <= CMD_PRE;
                // Precharge all
                s_addr <= autorefresh_pending ? 13'b0_0100_0000_0000 : 13'b0;
                s_banksel <= h_banksel;
            end
            default: ;
            endcase
        end
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset)
        h_rdata <= 16'b0;
    else
        h_rdata <= state == STATE_READ && timec == cas ? s_data : 16'b0;
end

always_ff @(posedge clk or posedge reset)
    if (reset)
        h_compl <= 1'b0;
    else
        h_compl <=  ((state == STATE_READ && timec == cas) ||
                     (state == STATE_WRITE && timec == tRP));

always_ff @(posedge clk) begin
    if (state == STATE_IDLE) begin
        outdata <= h_wdata[15:0];
        outbytesel <= h_wr_en ? ~h_bytesel[1:0] : 2'b00;
    end
end

always_ff @(posedge clk or posedge reset)
    if (reset)
        h_config_done <= 1'b0;
    else if (state == STATE_IDLE)
        h_config_done <= 1'b1;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        row_is_open <= 4'b0;
    end else begin
        if (state == STATE_ACT) begin
            row_is_open[h_banksel] <= 1'b1;
            open_row[h_banksel] <= h_rowsel;
        end else if (state == STATE_PCH)
            row_is_open[h_banksel] <= 1'b0;
        else if (state == STATE_AUTOREF)
            row_is_open <= 4'b0;
    end
end

always_ff @(posedge clk)
    state <= next_state;

endmodule
