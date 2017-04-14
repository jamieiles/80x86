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
           output logic [7:0] leds);

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

// Data bus
wire [19:1] data_m_addr;
wire [15:0] data_m_data_in;
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
wire [15:0] q_m_data_in;
wire q_m_ack;
wire q_m_access;
wire q_m_wr_en;
wire [1:0] q_m_bytesel;

wire d_io;

wire sdram_access = q_m_access & ~d_io;

wire leds_access = {data_m_addr[15:1], 1'b0} == 16'hfffe & data_m_access & d_io;
wire leds_ack;
wire io_ack = leds_ack;

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
                      .data_m_ack(data_mem_ack),
                      .*);

SysPLL	SysPLL(.refclk(clk),
	       .outclk_0(sys_clk),
               .outclk_1(sdr_clk));

SDRAMController #(.size(32 * 1024 * 1024),
                  .clkf(50000000))
                SDRAMController(.clk(sys_clk),
                                .reset(reset),
                                .cs(sdram_access),
                                .h_addr(q_m_addr),
                                .h_wdata(q_m_data_out),
                                .h_rdata(q_m_data_in),
                                .h_wr_en(q_m_wr_en),
                                .h_bytesel(q_m_bytesel),
                                .h_compl(q_m_ack),
                                .h_config_done(),
                                .*);

LEDSRegister     LEDSRegister(.cs(leds_access),
                              .leds_val(leds),
                              // Write to read port
                              .data_m_data_in(data_m_data_out),
                              .data_m_ack(leds_ack),
                              .*);

Core Core(.clk(sys_clk),
	  .lock(),
          .*);

endmodule
