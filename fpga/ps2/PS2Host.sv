module PS2Host #(parameter clkf=50000000)
                (input logic clk,
                 input logic reset,
                 // Host signals
                 output logic [7:0] rx,
                 output logic rx_valid,
                 output logic error,
                 input logic start_tx,
                 input logic [7:0] tx,
                 output logic tx_busy,
                 // Connector signals
                 inout ps2_clk,
                 inout ps2_dat);

localparam tx_clock_inhibit_reload = (clkf / 1000000) * 100;
localparam tx_clock_inhibit_bits = $clog2(tx_clock_inhibit_reload);
reg [tx_clock_inhibit_bits-1:0] clk_inhibit_count;

wire ps2_clk_sync;
wire ps2_dat_sync;

reg last_ps2_clk;
reg [2:0] bitpos;

reg [7:0] tx_reg;

typedef enum logic [3:0] {
    STATE_IDLE,
    // Receive
    STATE_RX,
    STATE_RX_PARITY,
    STATE_STOP,
    // Transmit
    STATE_INHIBIT,
    STATE_TX_START,
    STATE_TX,
    STATE_TX_PARITY,
    STATE_TX_STOP,
    STATE_TX_ACK
} state_t;

state_t state, next_state;

// Host samples on the negative edge.
wire do_sample = last_ps2_clk & ~ps2_clk_sync;

assign rx_valid = state == STATE_STOP && do_sample;

reg dat_o;
reg drive_dat;

assign ps2_clk = state == STATE_INHIBIT ? 1'b0 : 1'bz;
assign ps2_dat = drive_dat ? dat_o : 1'bz;

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
    STATE_RX: next_state = do_sample && bitpos == 3'd7 ? STATE_RX_PARITY : STATE_RX;
    STATE_RX_PARITY: next_state = do_sample ? STATE_STOP : STATE_RX_PARITY;
    STATE_STOP: next_state = do_sample ? STATE_IDLE : STATE_STOP;
    STATE_INHIBIT: next_state = ~|clk_inhibit_count ? STATE_TX_START : STATE_INHIBIT;
    STATE_TX_START: next_state = STATE_TX;
    STATE_TX: next_state = do_sample && bitpos == 3'd7 ? STATE_TX_PARITY : STATE_TX;
    STATE_TX_PARITY: next_state = do_sample ? STATE_TX_STOP : STATE_TX_PARITY;
    STATE_TX_STOP: next_state = do_sample ? STATE_TX_ACK : STATE_TX_STOP;
    STATE_TX_ACK: next_state = do_sample ? STATE_IDLE : STATE_TX_ACK;
    default: next_state = STATE_IDLE;
    endcase

    if (start_tx)
        next_state = STATE_INHIBIT;

    if (reset)
        next_state = STATE_IDLE;
end

always_ff @(posedge clk)
    if (state == STATE_TX_START)
        dat_o <= 1'b0;
    else if (do_sample && state == STATE_TX)
        dat_o <= tx_reg[bitpos];
    else if (do_sample && state == STATE_TX_PARITY)
        dat_o <= ~^tx_reg;

always_comb
    case (state)
    STATE_TX_START: drive_dat = 1'b1;
    STATE_TX: drive_dat = 1'b1;
    STATE_TX_PARITY: drive_dat = 1'b1;
    default: drive_dat = 1'b0;
    endcase

always_ff @(posedge clk or posedge reset)
    if (reset)
        tx_busy <= 1'b0;
    else if (start_tx)
        tx_busy <= 1'b1;
    else if (state == STATE_IDLE)
        tx_busy <= 1'b0;

always_ff @(posedge clk)
    if (state == STATE_INHIBIT)
        clk_inhibit_count <= clk_inhibit_count - 1'b1;
    else
        clk_inhibit_count <= tx_clock_inhibit_bits'(tx_clock_inhibit_reload);

always_ff @(posedge clk)
    state <= next_state;

always_ff @(posedge clk)
    if (state == STATE_RX || state == STATE_TX)
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
    else if (state == STATE_RX_PARITY && do_sample)
        error <= ~^{rx, ps2_dat_sync};

always_ff @(posedge clk)
    if (start_tx)
        tx_reg <= tx;

endmodule
