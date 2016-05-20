import os
import sys

from py8086sim.Cpu import Sim

class Runner(object):
    binary = None

    def __init__(self):
        self.cpu = Sim()

        with open(os.path.join('${CMAKE_CURRENT_BINARY_DIR}', 'programs', self.binary)) as b:
            for offset, b in enumerate(b.read()):
                self.cpu.write_mem8(offset, ord(b))

    def run(self, max_cycles=1000):
        self.setup()

        for i in xrange(max_cycles):
            if self.cpu.has_trapped():
                self.int3_hook()
                self.validate_result()
                return
            self.cpu.cycle()

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

    def assert_mem_equal(self, address, expected, width=8):
        if width == 8:
            v = self.cpu.read_mem8(address)
        elif width == 16:
            v = self.cpu.read_mem16(address)
        else:
            raise ValueError('Invalid memory width')
        if v != expected:
            raise ValueError('mem[{0:x}]: {1:x} != {2:x}'.format(address, v, expected))

    def write_reg(self, reg, val):
        self.cpu.write_reg(reg, val)
