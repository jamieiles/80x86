#pragma once

#include "CPU.h"
#include "Display.h"

class CGA : public IOPorts {
public:
    const phys_addr buffer_phys = 0xb8000;

    CGA(Memory *mem)
        : IOPorts(0x03da, 1),
        mem(mem)
    {
    }

    void write8(uint16_t __unused port_num, unsigned __unused offs,
                uint8_t __unused v) {}
    uint8_t read8(uint16_t __unused port_num, unsigned __unused offs)
    {
        return 0x9;
    }

    void update();
private:
    Memory *mem;
    Display display;
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
