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

import os
import sys
from py8086sim.Cpu import GPR

from Runner import Runner

class ModrmOverrideRunner(Runner):
    binary = 'modrm_override.bin'

    def setup(self):
        self.write_reg(GPR.IP, 1024)
        self.cpu.write_mem16(0x2000, 0x200, 0)
        self.cpu.write_mem16(0x3000, 0x100, 0)

    def validate_result(self):
        self.assert_mem_equal(0x2000, 0x200, 0xa5a5, width=16)
        self.assert_mem_equal(0x3000, 0x100, 0xa0a0, width=16)

ModrmOverrideRunner(sys.argv[1]).run()
