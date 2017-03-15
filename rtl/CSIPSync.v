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
    if (reset || propagate) begin
        ip_updated <= 1'b0;
        cs_updated <= 1'b0;
    end

    if (ip_update && !ip_updated) begin
        ip <= new_ip;
        ip_updated <= 1'b1;
    end

    if (cs_update)
        cs_updated <= 1'b1;
end

endmodule
