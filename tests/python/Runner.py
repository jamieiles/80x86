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

class Runner(object):
    binary = None
    load_address = 0

    def __init__(self, simname):
        simtype = getattr(sys.modules['py8086sim.Cpu'], simname)
        self.cpu = simtype(self.binary)
        self.cpu.reset()
        self.cpu.write_reg(GPR.CS, 0x0000)

        with open(os.path.join('${CMAKE_CURRENT_BINARY_DIR}', 'programs', self.binary)) as b:
            bytevec = [ord(v) for v in b.read()]
            if len(bytevec) % 2 == 1:
                bytevec.append(0)
            wordvec = []
            for a, b in zip(bytevec[::2], bytevec[1::2]):
                wordvec.append(a | (b << 8))
            self.cpu.write_vector16(self.cpu.read_reg(GPR.CS),
                                    self.load_address, wordvec)

    def run(self, max_cycles=1000):
        self.setup()

        for i in xrange(max_cycles):
            if self.cpu.has_trapped():
                self.int3_hook()
                self.validate_result()
                return
            self.cpu.step()

        raise Exception('Test timed out')

    def setup(self):
        pass

    def int3_hook(self):
        pass

    def validate_result(self):
        pass

    def assert_reg_equal(self, regnum, expected):
        v = self.cpu.read_reg(regnum)
        if v != expected:
            raise ValueError('Register ' + str(regnum) + ': {0:x} != {1:x}'.format(v, expected))

    def assert_mem_equal(self, segment, address, expected, width=8):
        if width == 8:
            v = self.cpu.read_mem8(segment, address)
        elif width == 16:
            v = self.cpu.read_mem16(segment, address)
        else:
            raise ValueError('Invalid memory width')
        if v != expected:
            raise ValueError('mem[{0:x}]: {1:x} != {2:x}'.format(address, v, expected))

    def write_reg(self, reg, val):
        self.cpu.write_reg(reg, val)
