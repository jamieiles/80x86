import os
import sys
from py8086sim.Cpu import GPR

from Runner import Runner

class ModrmOverrideRunner(Runner):
    binary = 'modrm_override.bin'

    def setup(self):
        self.write_reg(GPR.IP, 1024)

    def validate_result(self):
        self.assert_mem_equal((0x2000 << 4) + 0x200, 0xa5a5, width=16)
        self.assert_mem_equal((0x3000 << 4) + 0x100, 0xa0a0, width=16)

ModrmOverrideRunner(sys.argv[1]).run()
