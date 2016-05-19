import os
from py8086sim.Cpu import GPR

from Runner import Runner

class CallsRunner(Runner):
    binary = 'calls_and_jumps.bin'

    def validate_result(self):
        self.assert_mem_equal(0x1000, 1, width=16)
        self.assert_mem_equal(0x1002, 2, width=16)
        self.assert_mem_equal(0x1004, 3, width=16)

CallsRunner().run()
