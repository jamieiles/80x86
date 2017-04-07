create_clock -period 20.000 -name clk clk
derive_pll_clocks

set sys_clk   "pll|sys_pll_inst|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk"
# Reset request
set_false_path -from [get_ports {rst_in}]
