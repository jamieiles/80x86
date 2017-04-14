module Transmitter(input logic clk,
                   input logic reset,
                   input logic clken,
                   input logic [7:0] din,
                   input logic wr_en,
                   output logic tx,
                   output logic tx_busy);

typedef enum logic [1:0] {
    STATE_IDLE,
    STATE_START,
    STATE_DATA,
    STATE_STOP
} state_t;

reg [7:0] data;
reg [2:0] bitpos;
state_t state;

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        state <= STATE_IDLE;
        data <= 8'b0;
        bitpos <= 3'b0;
        tx <= 1'b1;
    end else begin
        case (state)
        STATE_IDLE: begin
            if (wr_en) begin
                state <= STATE_START;
                data <= din;
                bitpos <= 3'h0;
            end
        end
        STATE_START: begin
            if (clken) begin
                tx <= 1'b0;
                state <= STATE_DATA;
            end
        end
        STATE_DATA: begin
            if (clken) begin
                if (bitpos == 3'h7)
                    state <= STATE_STOP;
                else
                    bitpos <= bitpos + 3'h1;
                tx <= data[bitpos];
            end
        end
        STATE_STOP: begin
            if (clken) begin
                tx <= 1'b1;
                state <= STATE_IDLE;
            end
        end
        default: begin
            tx <= 1'b1;
            state <= STATE_IDLE;
        end
        endcase
    end
end

assign tx_busy = (state != STATE_IDLE);

endmodule
