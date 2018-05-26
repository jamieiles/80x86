# Copyright Jamie Iles, 2017, 2018
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

set_time_format -unit ns -decimal_places 3

# JTAG
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdi]
set_input_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tms]
set_output_delay -clock { altera_reserved_tck } 20 [get_ports altera_reserved_tdo]
set_false_path -from [get_clocks {altera_reserved_tck}] -to [get_clocks {altera_reserved_tck}]

create_clock -period 20.000 -name clk clk
derive_pll_clocks

set sdram_pll "SysPLL|altpll_component|auto_generated|pll1|clk[0]"
set sys_clk   "SysPLL|altpll_component|auto_generated|pll1|clk[1]"
set vga_clk   "SysPLL|altpll_component|auto_generated|pll1|clk[2]"
set pit_clk   "SysPLL|altpll_component|auto_generated|pll1|clk[3]"

derive_clock_uncertainty

set_false_path -from [get_clocks $pit_clk] -to [get_clocks $sys_clk]

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

create_generated_clock \
        -name sdram_clk \
        -source $sdram_pll \
        [get_ports {sdr_clk}]

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

set_multicycle_path -setup -end -from sdram_clk -to [get_clocks $sys_clk] 2

# Reset request
set_false_path -from [get_ports {rst_in_n}]

# uart
set_false_path -from [get_ports uart_rx]
set_false_path -to [get_ports uart_tx]

# PS2
set_false_path -from [get_ports {ps2_clk ps2_dat}]
set_false_path -to [get_ports {ps2_clk ps2_dat}]
set_false_path -from [get_ports {ps2_clk_b ps2_dat_b}]
set_false_path -to [get_ports {ps2_clk_b ps2_dat_b}]

# LEDs
set_false_path -to [get_ports {leds[*]}]
