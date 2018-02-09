// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

`default_nettype none
module VirtualJTAG(input logic [1:0] ir_out,
                   input logic tdo,
                   output logic [1:0] ir_in,
                   output logic tck,
                   output logic tdi,
                   output logic virtual_state_cdr,
                   output logic virtual_state_cir,
                   output logic virtual_state_e1dr,
                   output logic virtual_state_e2dr,
                   output logic virtual_state_pdr,
                   output logic virtual_state_sdr,
                   output logic virtual_state_udr,
                   output logic virtual_state_uir);

sld_virtual_jtag	#(.sld_auto_instance_index("YES"),
                          .sld_instance_index(0),
                          .sld_ir_width(2),
                          .sld_sim_action(""),
                          .sld_sim_n_scan(0),
                          .sld_sim_total_length(0))
                        sld_virtual_jtag_component(.ir_out(ir_out),
                                                   .tdo(tdo),
                                                   .ir_in(ir_in),
                                                   .tck(tck),
                                                   .tdi(tdi),
                                                   .virtual_state_cdr(virtual_state_cdr),
                                                   .virtual_state_cir(virtual_state_cir),
                                                   .virtual_state_e1dr(virtual_state_e1dr),
                                                   .virtual_state_e2dr(virtual_state_e2dr),
                                                   .virtual_state_pdr(virtual_state_pdr),
                                                   .virtual_state_sdr(virtual_state_sdr),
                                                   .virtual_state_udr(virtual_state_udr),
                                                   .virtual_state_uir(virtual_state_uir),
                                                   .jtag_state_cdr(),
                                                   .jtag_state_cir(),
                                                   .jtag_state_e1dr(),
                                                   .jtag_state_e1ir(),
                                                   .jtag_state_e2dr(),
                                                   .jtag_state_e2ir(),
                                                   .jtag_state_pdr(),
                                                   .jtag_state_pir(),
                                                   .jtag_state_rti(),
                                                   .jtag_state_sdr(),
                                                   .jtag_state_sdrs(),
                                                   .jtag_state_sir(),
                                                   .jtag_state_sirs(),
                                                   .jtag_state_tlr(),
                                                   .jtag_state_udr(),
                                                   .jtag_state_uir(),
                                                   .tms());

endmodule
