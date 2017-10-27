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

class IRQRunner(Runner):
    binary = 'irq.bin'

    def setup(self):
        self.write_reg(GPR.IP, 1024)

    def validate_result(self):
        self.assert_reg_equal(GPR.BX, 0x1234)
        self.assert_reg_equal(GPR.CX, 0x5678)

IRQRunner(sys.argv[1]).run()
