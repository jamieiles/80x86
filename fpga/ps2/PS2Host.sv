module PS2Host(input logic clk,
               input logic reset,
               // Host signals
               output logic [7:0] rx,
               output logic rx_valid,
               output logic error,
               // Connector signals
               inout ps2_clk,
               inout ps2_dat);

wire ps2_clk_sync;
wire ps2_dat_sync;

reg last_ps2_clk;
reg [2:0] bitpos;

typedef enum logic [1:0] {
    STATE_IDLE,
    STATE_RX,
    STATE_PARITY,
    STATE_STOP
} state_t;

state_t state, next_state;

// Host samples on the negative edge.
wire do_sample = last_ps2_clk & ~ps2_clk_sync;

assign rx_valid = state == STATE_STOP && do_sample;

assign ps2_clk = 1'bz;
assign ps2_dat = 1'bz;

BitSync         ClkSync(.clk(clk),
                        .d(ps2_clk),
                        .q(ps2_clk_sync));

BitSync         DatSync(.clk(clk),
                        .d(ps2_dat),
                        .q(ps2_dat_sync));

always_ff @(posedge clk or posedge reset)
    if (reset)
        last_ps2_clk <= 1'b0;
    else
        last_ps2_clk <= ps2_clk_sync;

always_comb begin
    case (state)
    STATE_IDLE: next_state = do_sample && !ps2_dat_sync ? STATE_RX : STATE_IDLE;
    STATE_RX: next_state = do_sample && bitpos == 3'd7 ? STATE_PARITY : STATE_RX;
    STATE_PARITY: next_state = do_sample ? STATE_STOP : STATE_PARITY;
    STATE_STOP: next_state = do_sample ? STATE_IDLE : STATE_STOP;
    endcase

    if (reset)
        next_state = STATE_IDLE;
end

always_ff @(posedge clk)
    state <= next_state;

always_ff @(posedge clk)
    if (state == STATE_RX)
        bitpos <= do_sample ? bitpos + 1'b1 : bitpos;
    else
        bitpos <= 3'b0;

always_ff @(posedge clk)
    if (state == STATE_RX && do_sample)
        rx <= {ps2_dat_sync, rx[7:1]};

always_ff @(posedge clk or posedge reset)
    if (reset)
        error <= 1'b0;
    else if (state == STATE_IDLE)
        error <= 1'b0;
    else if (state == STATE_PARITY && do_sample)
        error <= ~^{rx, ps2_dat_sync};

endmodule
