module PS2Controller #(parameter clkf=50000000)
                      (input logic clk,
                       input logic reset,
                       // CPU port
                       input logic cs,
                       input logic data_m_access,
                       input logic data_m_wr_en,
                       output logic data_m_ack,
                       output logic [15:0] data_m_data_out,
                       input logic [15:0] data_m_data_in,
                       input logic [1:0] data_m_bytesel,
                       // Interrupt
                       output logic ps2_intr,
                       // PS/2 signals
                       inout ps2_clk,
                       inout ps2_dat);

wire do_read = data_m_access & cs & ~data_m_wr_en;
wire do_write = data_m_access & cs & data_m_wr_en;

wire [7:0] rx;
wire rx_valid;
wire [7:0] tx = data_m_data_in[7:0];
wire start_tx = do_write & data_m_bytesel[0];
wire tx_busy;
wire error;
wire empty;
wire full;
wire [7:0] fifo_rd_data;
wire fifo_wr_en = rx_valid & ~error & ~full;

wire fifo_rd_en = cs & data_m_wr_en & data_m_bytesel[1] & ~empty;

reg unread_error = 1'b0;

assign ps2_intr = ~empty;

Fifo    #(.data_width(8),
          .depth(8))
        Fifo(.rd_en(fifo_rd_en),
             .rd_data(fifo_rd_data),
             .wr_en(fifo_wr_en),
             .wr_data(rx),
             // verilator lint_off PINCONNECTEMPTY
             .nearly_full(),
             // verilator lint_on PINCONNECTEMPTY
             .*);

wire [7:0] status = {5'b0, tx_busy, unread_error, ~empty};
wire [7:0] data = empty ? 8'b0 : fifo_rd_data;

always_ff @(posedge clk)
    data_m_data_out <= do_read ? {status, data} : 16'b0;

always_ff @(posedge clk)
    data_m_ack <= data_m_access & cs;

always_ff @(posedge clk or posedge reset)
    if (reset)
        unread_error <= 1'b0;
    else if (rx_valid & error)
        unread_error <= 1'b1;
    else if (do_read & data_m_bytesel[1])
        unread_error <= 1'b0;

PS2Host #(.clkf(clkf))
        PS2Host(.*);

endmodule
