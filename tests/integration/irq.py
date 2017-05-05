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
