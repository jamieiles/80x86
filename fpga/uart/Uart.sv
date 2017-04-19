module Uart #(parameter clk_freq = 50000000)
             (input logic clk,
              input logic reset,
              input logic [7:0] din,
              input logic wr_en,
              output logic tx,
              output logic tx_busy,
              input logic rx,
              output logic rdy,
              input logic rdy_clr,
              output logic [7:0] dout);

wire rxclk_en, txclk_en;

BaudRateGen #(.clk_freq(clk_freq))
            BaudRateGen(.*);

Transmitter Transmitter(.clken(txclk_en),
                        .*);

Receiver Receiver(.clken(rxclk_en),
                  .data(dout),
                  .*);

endmodule
