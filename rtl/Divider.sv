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

// Non-restoring division
`default_nettype none
module Divider(input logic clk,
               input logic reset,
               input logic start,
               input logic is_8_bit,
               input logic is_signed,
               output logic busy,
               output logic complete,
               output logic error,
               input logic [31:0] dividend,
               input logic [15:0] divisor,
               output logic [15:0] quotient,
               output logic [15:0] remainder);

typedef enum bit[1:0] {
    INIT,
    WORKING,
    RESTORE,
    FIX_SIGN
} DivState_t;

DivState_t state, next_state;

// Remainder
wire [15:0] rem8 = {{8{P[15]}}, P[15:8]};
wire [15:0] rem16 = P[31:16];
assign remainder = is_8_bit ? rem8 : rem16;

// Shifted divisor
wire [31:0] udivshift8 = {16'b0, divisor[7:0], 8'b0};
wire [31:0] udivshift16 = {divisor, 16'b0};
wire [31:0] udivshift = is_8_bit ? udivshift8 : udivshift16;

wire [31:0] sdivshift8 = {16'b0, divisor_mag[7:0], 8'b0};
wire [31:0] sdivshift16 = {divisor_mag, 16'b0};
wire [31:0] sdivshift = is_8_bit ? sdivshift8 : sdivshift16;

wire [31:0] D = is_signed ? sdivshift : udivshift;

// Overflow
wire unsigned_overflow8 = dividend[15:0] >= {divisor[7:0], 8'b0};
wire unsigned_overflow16 = dividend >= {divisor, 16'b0};
wire unsigned_overflow = is_8_bit ? unsigned_overflow8 : unsigned_overflow16;

wire signed_overflow8 = in_signs_equal && dividend_mag[14:7] >= divisor_mag[7:0];
wire signed_overflow16 = in_signs_equal && dividend_mag[30:15] >= divisor_mag;
wire signed_overflow = is_8_bit ? signed_overflow8 : signed_overflow16;

wire overflow = is_signed ? signed_overflow : unsigned_overflow;
wire div_by_zero = divisor == 16'b0;

// Magnitudes
wire [31:0] dividend_mag = (dividend + {32{dividend[31]}}) ^ {32{dividend[31]}};
wire [15:0] divisor_mag = (divisor + {16{divisor[15]}}) ^ {16{divisor[15]}};

// Dividend
wire [63:0] P16 = is_signed ?
    {32'b0, dividend_mag} : {32'b0, dividend};
wire [63:0] P8 = is_signed ?
    {48'b0, dividend_mag[15:0]} : {48'b0, dividend[15:0]};
wire [63:0] P_init = is_8_bit ? P8 : P16;

// Sign bits
wire in_signs_equal8 = ~(dividend[15] ^ divisor[7]);
wire in_signs_equal16 = ~(dividend[31] ^ divisor[15]);
wire in_signs_equal = is_8_bit ? in_signs_equal8 : in_signs_equal16;
wire dividend_negative = is_8_bit ? dividend[15] : dividend[31];

reg [63:0] P;
reg [3:0] idx;
reg [15:0] restored_quotient;
wire [15:0] negative_quotient = ~quotient + 1'b1;

// Error condition
wire raise_error = div_by_zero | overflow;

assign busy = (start || (state != INIT) && !complete);

always_comb begin
    if (is_8_bit) begin
        restored_quotient = {8'b0, P[63] ?
            quotient[7:0] - ~quotient[7:0] - 8'b1 : quotient[7:0] - ~quotient[7:0]};
    end else begin
        restored_quotient = P[63] ?
            quotient - ~quotient - 16'b1 : quotient - ~quotient;
    end
end

always_comb begin
    case (state)
    INIT: next_state = start && !error && !raise_error ? WORKING : INIT;
    WORKING: next_state = idx == 4'b0 ? RESTORE : WORKING;
    RESTORE: next_state = is_signed ? FIX_SIGN : INIT;
    FIX_SIGN: next_state = INIT;
    endcase

    if (reset)
        next_state = INIT;
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        error <= 1'b0;
        complete <= 1'b0;
    end else begin
        case (state)
        INIT: begin
            error <= 1'b0;

            if (start) begin
                quotient <= 16'b0;
                P <= P_init;
                idx <= !is_8_bit ? 4'hf : 4'h7;
                error <= raise_error;
            end

            complete <= start && raise_error;
        end
        WORKING: begin
            if (!P[63]) begin
                quotient[idx] <= 1'b1;
                P <= (P * 2) - {32'b0, D};
            end else begin
                P <= (P * 2) + {32'b0, D};
            end
            idx <= idx - 1'b1;
        end
        RESTORE: begin
            quotient <= restored_quotient;
            if (P[63])
                P <= P + {{32{D[31]}}, D};
            complete <= ~is_signed;
        end
        FIX_SIGN: begin
            if (~in_signs_equal) begin
                quotient <= negative_quotient;
                error <= |quotient & is_signed & ~negative_quotient[is_8_bit ? 7 : 15];
            end else begin
                error <= |quotient & is_signed & quotient[is_8_bit ? 7 : 15];
            end
            if (dividend_negative && is_8_bit)
                P[15:8] <= ~P[15:8] + 1'b1;
            else if (dividend_negative)
                P[31:16] <= ~P[31:16] + 1'b1;
            complete <= 1'b1;
        end
        endcase
    end
end

always_ff @(posedge clk)
    state <= next_state;

endmodule
