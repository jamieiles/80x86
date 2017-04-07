module Top(input logic clk,
	   input logic rst_in);

wire sys_clk;
reg reset;

always_ff @(posedge sys_clk)
    reset <= rst_in;

SysPLL	SysPLL(.refclk(clk),
	       .rst(1'b0),
	       .outclk_0(sys_clk));

Core Core(.clk(sys_clk),
	  .reset(reset),
	  .instr_m_addr(),
	  .instr_m_data_in(),
	  .instr_m_access(),
	  .instr_m_ack(),
	  .data_m_addr(),
	  .data_m_data_in(),
	  .data_m_data_out(),
	  .data_m_access(),
	  .data_m_ack(),
	  .data_m_wr_en(),
	  .data_m_bytesel(),
	  .d_io(),
	  .lock(),
	  .debug_stopped(),
	  .debug_seize(),
	  .debug_addr(),
	  .debug_run(),
	  .debug_val(),
	  .debug_wr_val(),
	  .debug_wr_en());

endmodule
