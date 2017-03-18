import os
import sys
from py8086sim.Cpu import GPR

from Runner import Runner

class MovesRunner(Runner):
    binary = 'moves.bin'

    def setup(self):
        self.write_reg(GPR.IP, 1024)

    def validate_result(self):
        self.assert_mem_equal(0x1234, 0xabcd, width=16)
        self.assert_mem_equal(0x007e, 0xabcd, width=16)
        self.assert_reg_equal(GPR.AX, 0xabcd)

MovesRunner(sys.argv[1]).run()
