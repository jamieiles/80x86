import os
import sys
from py8086sim.Cpu import GPR

from Runner import Runner

class SingleStepRunner(Runner):
    binary = 'singlestep.bin'

    def setup(self):
        self.write_reg(GPR.IP, 1024)

    def validate_result(self):
        self.assert_reg_equal(GPR.BX, 5)
        self.assert_reg_equal(GPR.CX, 1)

SingleStepRunner(sys.argv[1]).run()
