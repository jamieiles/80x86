module SyncPulse(input logic clk,
                 input logic d,
                 output logic p,
                 output logic q);

wire synced;
reg last_val;

assign p = synced ^ last_val;
assign q = last_val;

always_ff @(posedge clk)
    last_val <= synced;

BitSync         BitSync(.clk(clk),
                        .d(d),
                        .q(synced));
endmodule
