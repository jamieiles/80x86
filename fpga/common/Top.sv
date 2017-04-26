module Top(input logic clk,
	   input logic rst_in_n,
           output logic s_ras_n,
           output logic s_cas_n,
           output logic s_wr_en,
           output logic [1:0] s_bytesel,
           output logic [12:0] s_addr,
           output logic s_cs_n,
           output logic s_clken,
           inout [15:0] s_data,
           output logic [1:0] s_banksel,
           output logic sdr_clk,
`ifdef CONFIG_LEDS
           output logic [7:0] leds,
`endif // CONFIG_LEDS
           input logic uart_rx,
           output logic uart_tx,
           output logic spi_sclk,
           output logic spi_mosi,
           input logic spi_miso,
           output logic spi_ncs);

wire sys_clk;
wire reset_n;
wire reset = ~reset_n | debug_reset;

wire [1:0] ir;
wire tdo;
wire tck;
wire tdi;
wire sdr;
wire cdr;
wire udr;
wire debug_stopped;
wire debug_seize;
wire debug_reset;
wire debug_run;
wire [7:0] debug_addr;
wire [15:0] debug_wr_val;
wire [15:0] debug_val;
wire debug_wr_en;

wire [15:0] io_data = sdram_config_data | uart_data | spi_data;
wire [15:0] mem_data;

// Data bus
wire [19:1] data_m_addr;
wire [15:0] data_m_data_in = d_io ? io_data : mem_data;
wire [15:0] data_m_data_out;
wire data_m_access;
wire data_m_ack = data_mem_ack | io_ack;
wire data_m_wr_en;
wire [1:0] data_m_bytesel;

// Instruction bus
wire [19:1] instr_m_addr;
wire [15:0] instr_m_data_in;
wire instr_m_access;
wire instr_m_ack;

// Multiplexed I/D bus.
wire [19:1] q_m_addr;
wire [15:0] q_m_data_out;
wire [15:0] q_m_data_in = sdram_data | bios_data;
wire q_m_ack = sdram_ack | bios_ack;
wire q_m_access;
wire q_m_wr_en;
wire [1:0] q_m_bytesel;

wire d_io;

wire sdram_access;
wire sdram_ack;
wire [15:0] sdram_data;

`ifdef CONFIG_LEDS
wire leds_access;
wire leds_ack;
`endif // CONFIG_LEDS

wire bios_access;
wire bios_ack;
wire [15:0] bios_data;

wire sdram_config_access;
wire sdram_config_ack;
wire sdram_config_done;
wire [15:0] sdram_config_data;

wire uart_access;
wire uart_ack;
wire [15:0] uart_data;

wire spi_access;
wire spi_ack;
wire [15:0] spi_data;

wire default_io_access;
wire default_io_ack;

wire io_ack = sdram_config_ack |
              default_io_ack |
              uart_ack |
`ifdef CONFIG_LEDS
              leds_ack |
`endif // CONFIG_LEDS
              spi_ack;

always_ff @(posedge clk)
    default_io_ack <= default_io_access;

always_comb begin
`ifdef CONFIG_LEDS
    leds_access = 1'b0;
`endif // CONFIG_LEDS
    sdram_config_access = 1'b0;
    default_io_access = 1'b0;
    uart_access = 1'b0;
    spi_access = 1'b0;

    if (d_io && data_m_access) begin
        casez ({data_m_addr[15:1], 1'b0})
`ifdef CONFIG_LEDS
        16'b1111_1111_1111_1110: leds_access = 1'b1;
`endif // CONFIG_LEDS
        16'b1111_1111_1111_1100: sdram_config_access = 1'b1;
        16'b1111_1111_1111_1010: uart_access = 1'b1;
        16'b1111_1111_1111_00z0: spi_access = 1'b1;
        default:  default_io_access = 1'b1;
        endcase
    end
end

always_comb begin
    sdram_access = 1'b0;
    bios_access = 1'b0;

    if (q_m_access) begin
        casez ({q_m_addr, 1'b0})
        20'b1111_111?_????_????_????: bios_access = 1'b1;
        default: sdram_access = 1'b1;
        endcase
    end
end

wire data_mem_ack;

BitSync         ResetSync(.clk(sys_clk),
                          .d(rst_in_n),
                          .q(reset_n));

VirtualJTAG VirtualJTAG(.ir_out(),
                        .tdo(tdo),
                        .ir_in(ir),
                        .tck(tck),
                        .tdi(tdi),
                        .virtual_state_sdr(sdr),
                        .virtual_state_e1dr(),
                        .virtual_state_cdr(cdr),
                        .virtual_state_udr(udr));

JTAGBridge      JTAGBridge(.cpu_clk(sys_clk),
                           .*);

MemArbiter MemArbiter(.clk(sys_clk),
                      .data_m_data_in(mem_data),
                      .data_m_access(data_m_access & ~d_io),
                      .data_m_ack(data_mem_ack),
                      .*);

SDRAMController #(.size(`CONFIG_SDRAM_SIZE),
                  .clkf(50000000))
                SDRAMController(.clk(sys_clk),
                                .reset(reset),
                                .data_m_access(q_m_access),
                                .cs(sdram_access),
                                .h_addr(q_m_addr),
                                .h_wdata(q_m_data_out),
                                .h_rdata(sdram_data),
                                .h_wr_en(q_m_wr_en),
                                .h_bytesel(q_m_bytesel),
                                .h_compl(sdram_ack),
                                .h_config_done(sdram_config_done),
                                .*);

BIOS #(.depth(4096))
     BIOS(.clk(sys_clk),
          .cs(bios_access),
          .data_m_access(q_m_access),
          .data_m_ack(bios_ack),
          .data_m_addr(q_m_addr),
          .data_m_wr_en(q_m_wr_en),
          .data_m_data_out(bios_data),
          .data_m_data_in(q_m_data_out),
          .data_m_bytesel(q_m_bytesel));

SDRAMConfigRegister SDRAMConfigRegister(.clk(sys_clk),
                                        .cs(sdram_config_access),
                                        .data_m_ack(sdram_config_ack),
                                        .data_m_data_out(sdram_config_data),
                                        .config_done(sdram_config_done),
                                        .*);

`ifdef CONFIG_LEDS
LEDSRegister     LEDSRegister(.clk(sys_clk),
                              .cs(leds_access),
                              .leds_val(leds),
                              .data_m_data_in(data_m_data_out),
                              .data_m_ack(leds_ack),
                              .*);
`endif // CONFIG_LEDS

UartPorts #(.clk_freq(50000000))
          UartPorts(.clk(sys_clk),
                    .rx(uart_rx),
                    .tx(uart_tx),
                    .cs(uart_access),
                    .data_m_ack(uart_ack),
                    .data_m_data_out(uart_data),
                    .data_m_data_in(data_m_data_out),
                    .*);

SPIPorts SPIPorts(.clk(sys_clk),
                  .cs(spi_access),
                  .data_m_ack(spi_ack),
                  .data_m_data_out(spi_data),
                  .data_m_data_in(data_m_data_out),
                  .data_m_addr(data_m_addr[1]),
                  .miso(spi_miso),
                  .mosi(spi_mosi),
                  .sclk(spi_sclk),
                  .ncs(spi_ncs),
                  .*);

SysPLL	SysPLL(.refclk(clk),
	       .rst(1'b0),
               .locked(),
               .*);

Core Core(.clk(sys_clk),
	  .lock(),
          .*);

endmodule
