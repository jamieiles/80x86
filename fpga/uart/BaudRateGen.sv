module BaudRateGen #(parameter clk_freq = 50000000)
                    (input logic clk,
                     input logic reset,
                     output logic rxclk_en,
                     output logic txclk_en);

localparam RX_ACC_MAX = clk_freq / (115200 * 16);
localparam TX_ACC_MAX = clk_freq / 115200;
localparam RX_ACC_WIDTH = $clog2(RX_ACC_MAX);
localparam TX_ACC_WIDTH = $clog2(TX_ACC_MAX);
reg [RX_ACC_WIDTH - 1:0] rx_acc = 0;
reg [TX_ACC_WIDTH - 1:0] tx_acc = 0;

assign rxclk_en = (rx_acc == 5'd0);
assign txclk_en = (tx_acc == 9'd0);

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        rx_acc <= {RX_ACC_WIDTH-1{1'b0}};
    end else begin
        if (rx_acc == RX_ACC_MAX[RX_ACC_WIDTH - 1:0])
            rx_acc <= {RX_ACC_WIDTH-1{1'b0}};
        else
            rx_acc <= rx_acc + 1'b1;
    end
end

always_ff @(posedge clk or posedge reset) begin
    if (reset) begin
        tx_acc <= {TX_ACC_WIDTH-1{1'b0}};
    end else begin
        if (tx_acc == TX_ACC_MAX[TX_ACC_WIDTH - 1:0])
            tx_acc <= {TX_ACC_WIDTH-1{1'b0}};
        else
            tx_acc <= tx_acc + 1'b1;
    end
end

endmodule
