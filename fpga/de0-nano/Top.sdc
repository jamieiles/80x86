# JTAG
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdi]
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tms]
set_output_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdo]
set_false_path -from [get_clocks {altera_reserved_tck}] -to [get_clocks {altera_reserved_tck}]

create_clock -period 20.000 -name clk clk
derive_pll_clocks
derive_clock_uncertainty

set sys_clk   "pll|altpll_component|auto_generated|pll1|clk[0]"
# Reset request
set_false_path -from [get_ports {rst_in_n}]
