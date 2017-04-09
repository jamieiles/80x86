module Top(input logic clk,
	   input logic rst_in_n);

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

SysPLL	SysPLL(.refclk(clk),
	       .outclk_0(sys_clk));

reg instr_m_ack;
wire instr_m_access;
always_ff @(posedge sys_clk)
    instr_m_ack <= instr_m_access;

reg data_m_ack;
wire data_m_access;
always_ff @(posedge sys_clk)
    data_m_ack <= data_m_access;

Core Core(.clk(sys_clk),
	  .reset(reset),
	  .instr_m_addr(),
	  .instr_m_data_in(),
	  .instr_m_access,
	  .instr_m_ack,
	  .data_m_addr(),
	  .data_m_data_in(),
	  .data_m_data_out(),
	  .data_m_access,
	  .data_m_ack,
	  .data_m_wr_en(),
	  .data_m_bytesel(),
	  .d_io(),
	  .lock(),
	  .debug_stopped,
	  .debug_seize,
	  .debug_addr,
	  .debug_run,
	  .debug_val,
	  .debug_wr_val,
	  .debug_wr_en);

endmodule
