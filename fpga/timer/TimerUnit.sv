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

`default_nettype none
module TimerUnit(input logic clk,
                 input logic reset,
                 input logic pit_clk,
                 input logic [7:0] reload_in,
                 input logic load,
                 input logic [1:0] rw_in,
                 input logic [1:0] mode_in,
                 input logic configure,
                 input logic latch_count,
                 input logic read_count,
                 output logic [7:0] count_out,
                 input logic gate,
                 output logic out);

wire pit_clk_posedge = pit_clk & ~last_pit_clk;
reg last_pit_clk;
reg [15:0] count, reload, latched_count;

reg [1:0] rw;
reg [1:0] mode;
reg [1:0] latched;
reg [7:0] reload_low;
reg reload_low_stored;
reg access_low;
reg last_gate;

wire trigger = gate & ~last_gate;

task count_rate_gen;
begin
    if (trigger) begin
        count <= reload[0] ? reload - 1'b1 : reload;
        out <= 1'b1;
    end else if (pit_clk_posedge && gate) begin
        if (count == 16'b0 || count == 16'b1)
            count <= reload[0] ? reload - 16'd1 : reload;
        else
            count <= count - 16'd1;
        if (count == 16'd2)
            out <= ~out;
    end
end
endtask

task count_square_wave;
begin
    if (trigger) begin
        count <= reload - 1'b1;
        out <= 1'b1;
    end else begin
        if (pit_clk_posedge && gate)
            count <= (count == 16'b0 ? reload : count) - 1'b1;
    
        if (count == 16'b1)
            out <= 1'b0;
        else
            out <= 1'b1;
    end
end
endtask

always_ff @(posedge clk) begin
    last_pit_clk <= pit_clk;
    last_gate <= gate;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        latched_count <= 16'b0;
        access_low <= 1'b0;
        latched <= 2'b00;
    end else begin
        if (read_count && rw == 2'b11) begin
            latched_count <= {8'b0, latched_count[15:8]};
            latched <= {1'b0, latched[1]};
        end

        if (latch_count) begin
            latched_count <= count;
            latched <= 2'b11;
            access_low <= rw != 2'b10;
        end

        if (load && rw == 2'b11)
            access_low <= 1'b1;
    end
end

always_comb begin
    if (|latched)
        count_out = latched_count[7:0];
    else
        count_out = access_low ? count[7:0] : count[15:8];
end

always_ff @(posedge reset or posedge clk) begin
    if (reset) begin
        rw <= 2'b0;
        mode <= 2'b0;
    end else if (configure) begin
        mode <= mode_in;
        rw <= rw_in;
    end
end

always_ff @(posedge reset or posedge clk) begin
    if (reset) begin
        reload <= 16'b0;
        reload_low_stored <= 1'b0;
    end else if (configure) begin
        reload <= 16'b0;
        reload_low_stored <= 1'b0;
    end else if (load) begin
        if (rw == 2'b10) begin
            reload[15:8] <= reload_in;
        end else if (rw == 2'b01) begin
            reload[7:0] <= reload_in;
        end else begin
            if (reload_low_stored) begin
                reload <= {reload_in, reload_low};
                reload_low_stored <= 1'b0;
            end else begin
                reload_low <= reload_in;
                reload_low_stored <= 1'b1;
            end
        end
    end
end

always_ff @(posedge reset or posedge clk) begin
    if (reset) begin
        count <= 16'b0;
        out <= 1'b1;
    end else begin
        if (mode[1:0] == 2'b11)
            count_rate_gen();
        else if (mode[1:0] == 2'b10)
            count_square_wave();

        if (!gate)
            out <= 1'b1;
    end
end

endmodule
