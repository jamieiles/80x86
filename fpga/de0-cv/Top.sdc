# Copyright Jamie Iles, 2017
#
# This file is part of s80x86.
#
# s80x86 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# s80x86 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

# JTAG
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdi]
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tms]
set_output_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdo]
set_false_path -from [get_clocks {altera_reserved_tck}] -to [get_clocks {altera_reserved_tck}]

create_clock -period 20.000 -name clk clk
derive_pll_clocks

set sdram_pll "SysPLL|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk"
set sys_clk   "SysPLL|altera_pll_i|general[1].gpll~PLL_OUTPUT_COUNTER|divclk"

# SPI clock
create_generated_clock -name {spi_clk} \
        -source [get_pins {SysPLL|altera_pll_i|general[1].gpll~PLL_OUTPUT_COUNTER|divclk}] \
        -divide_by 2 -master_clock {SysPLL|altera_pll_i|general[1].gpll~PLL_OUTPUT_COUNTER|divclk} \
        [get_registers {SPIPorts:SPIPorts|SPIMaster:SPIMaster|sclk}]

derive_clock_uncertainty

# SDRAM
set sdram_tsu       1.5
set sdram_th        0.8
set sdram_tco_min   2.7
set sdram_tco_max   6.4

# FPGA timing constraints
set sdram_input_delay_min        $sdram_tco_min
set sdram_input_delay_max        $sdram_tco_max
set sdram_output_delay_min      -$sdram_th
set sdram_output_delay_max       $sdram_tsu

set_false_path -to [get_ports {sdr_clk}]

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
	-clock [get_clocks $sdram_pll] \
	-min $sdram_output_delay_min \
	$sdram_outputs
set_output_delay \
	-clock [get_clocks $sdram_pll] \
	-max $sdram_output_delay_max \
	$sdram_outputs

# FPGA Inputs
set sdram_inputs [get_ports {
	s_data[*]
}]
set_input_delay \
	-clock [get_clocks $sdram_pll] \
	-min $sdram_input_delay_min \
	$sdram_inputs
set_input_delay \
	-clock [get_clocks $sdram_pll] \
	-max $sdram_input_delay_max \
	$sdram_inputs

# SDRAM-to-FPGA multi-cycle constraint
#
# * The PLL is configured so that SDRAM clock leads the system
#   clock by -2.79ns
set_multicycle_path -setup -end -from [get_clocks $sdram_pll] -to [get_clocks $sys_clk] 2

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

# PS2
set_false_path -from [get_ports {ps2_clk ps2_dat}]
set_false_path -to [get_ports {ps2_clk ps2_dat}]

# LEDs
set_false_path -to [get_ports {leds[*]}]
