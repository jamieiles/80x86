# JTAG
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdi]
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tms]
set_output_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdo]
set_false_path -from [get_clocks {altera_reserved_tck}] -to [get_clocks {altera_reserved_tck}]

create_clock -period 20.000 -name clk clk
derive_pll_clocks

set sys_clk   "SysPLL|altpll_component|auto_generated|pll1|clk[0]"
set sdram_pll "SysPLL|altpll_component|auto_generated|pll1|clk[1]"

# SPI clock
create_generated_clock -name {spi_clk} \
        -source [get_pins {SysPLL|altpll_component|auto_generated|pll1|clk[0]}] \
        -divide_by 2 -master_clock {SysPLL|altpll_component|auto_generated|pll1|clk[0]} \
        [get_registers {SPIPorts:SPIPorts|SPIMaster:SPIMaster|sclk}]
# SDRAM PLL
create_generated_clock \
	-name sdram_clk \
	-source $sdram_pll \
	[get_ports {sdr_clk}]

derive_clock_uncertainty

# SDRAM
set sdram_tsu       1.5
set sdram_th        0.8
set sdram_tco_min   2.7
set sdram_tco_max   5.4

# FPGA timing constraints
set sdram_input_delay_min        $sdram_tco_min
set sdram_input_delay_max        $sdram_tco_max
set sdram_output_delay_min      -$sdram_th
set sdram_output_delay_max       $sdram_tsu

# PLL to FPGA output (clear the unconstrained path warning)
set_min_delay -from $sdram_pll -to [get_ports {sdr_clk}] 1
set_max_delay -from $sdram_pll -to [get_ports {sdr_clk}] 6

# FPGA Outputs
set sdram_outputs [get_ports {
	s_clken
	s_ras_n
	s_cas_n
	s_wr_en
	s_bytesel[*]
	s_addr[*]
	s_cs_n
	s_data[*]
	s_banksel[*]
}]
set_output_delay \
	-clock sdram_clk \
	-min $sdram_output_delay_min \
	$sdram_outputs
set_output_delay \
	-clock sdram_clk \
	-max $sdram_output_delay_max \
	$sdram_outputs

# FPGA Inputs
set sdram_inputs [get_ports {
	s_data[*]
}]
set_input_delay \
	-clock sdram_clk \
	-min $sdram_input_delay_min \
	$sdram_inputs
set_input_delay \
	-clock sdram_clk \
	-max $sdram_input_delay_max \
	$sdram_inputs

# SDRAM-to-FPGA multi-cycle constraint
#
# * The PLL is configured so that SDRAM clock leads the system
#   clock by -2.79ns
set_multicycle_path -setup -end -from sdram_clk -to $sys_clk 2

# Reset request
set_false_path -from [get_ports {rst_in_n}]

# uart
set_false_path -from [get_ports uart_rx]
set_false_path -to [get_ports uart_tx]

# SPI bus
set spi_delay_max 1
set spi_delay_min 1

# MOSI
set_output_delay -add_delay -clock {spi_clk} -max [expr $spi_delay_max] [get_ports {spi_mosi}]
set_output_delay -add_delay -clock {spi_clk} -min [expr $spi_delay_min] [get_ports {spi_mosi}]
# MISO
set_input_delay -add_delay -clock_fall -clock {spi_clk} -max [expr $spi_delay_max] [get_ports {spi_miso}]
set_input_delay -add_delay -clock_fall -clock {spi_clk} -min [expr $spi_delay_min] [get_ports {spi_miso}]
# CLK
set_output_delay -add_delay -clock {spi_clk} -max [expr $spi_delay_max] [get_ports {spi_sclk}]
set_output_delay -add_delay -clock {spi_clk} -min [expr $spi_delay_min] [get_ports {spi_sclk}]

set_multicycle_path -setup -start -from [get_clocks $sys_clk] -to [get_clocks {spi_clk}] 1
set_multicycle_path -hold -start -from [get_clocks $sys_clk] -to [get_clocks {spi_clk}] 1
set_multicycle_path -setup -end -from [get_clocks {spi_clk}] -to [get_clocks $sys_clk] 1
set_multicycle_path -hold -end -from [get_clocks {spi_clk}] -to [get_clocks $sys_clk] 1

set_false_path -to [get_ports {spi_ncs}]
