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

module Timer(input logic clk,
             input logic reset,
             input logic pit_clk,
             input logic cs,
             input logic [1:1] data_m_addr,
             input logic [15:0] data_m_data_in,
             output logic [15:0] data_m_data_out,
             input logic [1:0] data_m_bytesel,
             input logic data_m_wr_en,
             input logic data_m_access,
             output logic data_m_ack,
             output logic intr);

wire pit_clk_sync;
wire pit_clk_posedge = pit_clk_sync & ~last_pit_clk;
wire access_data = cs & data_m_access & ~data_m_addr[1] & data_m_bytesel[0];
wire access_ctrl = cs & data_m_access & data_m_addr[1] & data_m_bytesel[1];
wire [7:0] ctrl_wr_val = data_m_data_in[15:8];
wire [1:0] channel = ctrl_wr_val[7:6];

reg last_pit_clk;

reg [15:0] count, reload, latched_count;
reg [1:0] rw;
reg [2:0] mode;
reg [1:0] latched;
reg access_low;
reg reloaded;

BitSync PITSync(.clk(clk),
                .d(pit_clk),
                .q(pit_clk_sync));

always_ff @(posedge clk)
    last_pit_clk <= pit_clk_sync;

always_ff @(posedge clk)
    if (access_data && !data_m_wr_en) begin
        if (|latched) begin
            data_m_data_out <= {8'b0, latched_count[7:0]};
        end else
            data_m_data_out <= {8'b0, rw[0] ? count[7:0] : count[15:8]};
    end else begin
        data_m_data_out <= 16'b0;
    end

always_ff @(posedge reset or posedge clk) begin
    if (reset) begin
        {rw, mode, access_low} <= 6'b0;
        latched_count <= 16'b0;
        latched <= 2'b00;
    end else if (access_ctrl && data_m_wr_en && channel == 2'b00) begin
        if (ctrl_wr_val[5:4] == 2'b00 && ~|latched) begin
            latched <= 2'b11;
            latched_count <= count;
        end else begin
            mode <= ctrl_wr_val[3:1];
            access_low <= ctrl_wr_val[4];
            rw <= ctrl_wr_val[5:4];
        end
    end else if (access_data && data_m_wr_en && rw == 2'b11)
        access_low <= ~access_low;
    else if (access_data && !data_m_wr_en) begin
        latched <= {1'b0, latched[1]};
        latched_count <= {8'b0, latched_count[15:8]};
    end
end

always_ff @(posedge reset or posedge clk) begin
    if (reset) begin
        reload <= 16'b0;
        reloaded <= 1'b0;
    end else begin
        reloaded <= 1'b0;

        if (access_data && data_m_wr_en) begin
            if (access_low)
                reload[7:0] <= data_m_data_in[7:0];
            else begin
                reload[15:8] <= data_m_data_in[7:0];
                reloaded <= 1'b1;
            end
        end
    end
end

always_ff @(posedge reset or posedge clk) begin
    if (reset) begin
        count <= 16'b0;
        intr <= 1'b0;
    end else begin
        if (reloaded)
            count <= reload - 1'b1;
        else if (pit_clk_posedge && mode[1] == 1'b1) begin
            count <= (count == 16'b0 ? reload : count) - 1'b1;
            if (count == 16'b1)
                intr <= 1'b0;
            else
                intr <= 1'b1;
        end
    end
end

always_ff @(posedge clk)
    data_m_ack <= cs & data_m_access;

endmodule
