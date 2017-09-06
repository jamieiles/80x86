#pragma once

#include "CPU.h"
#include "Display.h"

class CGA : public IOPorts {
public:
    const phys_addr buffer_phys = 0xb8000;

    CGA(Memory *mem)
        : IOPorts(0x03d4, 8),
        mem(mem),
        reg_idx(0),
        status(0)
    {
        memset(idx_regs, 0, sizeof(idx_regs));
    }

    void write8(uint16_t __unused port_num, unsigned __unused offs,
                uint8_t __unused v)
    {
        if (port_num == 0 && offs == 0) {
            reg_idx = v;
        } else if (port_num == 0 && offs == 1) {
            idx_regs[reg_idx] = v;
        }
    }

    uint8_t read8(uint16_t __unused port_num, unsigned __unused offs)
    {
        if (port_num == 0)
            return offs == 0 ? reg_idx : idx_regs[reg_idx];
        else if (port_num == 6 && offs == 0) {
            status ^= 0x1;
            return status;
        }

        return 0;
    }

    void update();
private:
    Memory *mem;
    Display display;
    uint8_t reg_idx;
    uint8_t idx_regs[256];
    uint8_t status;
};

void CGA::update()
{
    int cols = 80;
    int rows = 25;

    for (auto row = 0; row < rows; ++row) {
        for (auto col = 0; col < cols; ++col) {
            auto addr = buffer_phys + ((row * cols) + col) * sizeof(uint16_t);
            auto char_attr = mem->read<uint16_t>(addr);

            display.set_cursor(row, col);
            display.write_char(char_attr);
        }
    }
    display.refresh();
}
