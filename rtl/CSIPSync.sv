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

// Synchronize CS and IP updates so that they can be deployed to the
// prefetcher atomically.  For something like a far jump, the microcode would
// first update the IP and then update CS.  If this happens during the
// microcode execution then potentially the prefetcher can start prefetching
// from an invalid address - CS remains the same but IP changes and we fetch
// from invalid/unmapped memory.
//
// This simple module just defers the output until propagation is ready, in
// this case, on completion of the microinstruction when CS:IP can be
// delivered to the prefetcher in a single cycle.
`default_nettype none
module CSIPSync(input logic clk,
                input logic reset,
                input logic cs_update,
                input logic ip_update,
                input logic [15:0] ip_in,
                input logic [15:0] new_ip,
                input logic propagate,
                output logic [15:0] ip_out,
                output logic update_out);

reg [15:0] ip;
reg ip_updated;
reg cs_updated;

assign ip_out = ip_updated ? ip : ip_update ? new_ip : ip_in;
assign update_out = propagate &
    (ip_updated | cs_updated | cs_update | ip_update);

always @(posedge clk or posedge reset) begin
    if (reset) begin
        ip_updated <= 1'b0;
        cs_updated <= 1'b0;
    end else begin
        if (propagate) begin
            ip_updated <= 1'b0;
            cs_updated <= 1'b0;
        end

        if (ip_update && !ip_updated && !propagate) begin
            ip <= new_ip;
            ip_updated <= 1'b1;
        end

        if (cs_update && !propagate)
            cs_updated <= 1'b1;
    end
end

endmodule
