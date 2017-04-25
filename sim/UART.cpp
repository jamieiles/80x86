#include <iostream>
#include <stdint.h>

#include "UART.h"
#include "CPU.h"

UART::UART()
    : IOPorts(0xfffa, 1)
{}

void UART::write8(uint16_t __unused port_num, unsigned offs, uint8_t v)
{
    if (offs == 0) {
        std::cout << static_cast<char>(v);
        std::cout.flush();
    }
}

void UART::write16(uint16_t __unused port_num, uint16_t v)
{
    write8(port_num, 0, v);
    write8(port_num, 1, v >> 8);
}

uint8_t UART::read8(uint16_t __unused port_num, unsigned offs)
{
    if (offs == 0) {
        if (!charbuf.size())
            return 0;

        auto v = charbuf[0];
        charbuf.pop_front();

        return v;
    } else {
        return !!charbuf.size();
    }
}

uint16_t UART::read16(uint16_t __unused port_num)
{
    return read8(0, 0) | (static_cast<uint16_t>(read8(0, 1)) << 8);
}

void UART::add_char(int c)
{
    charbuf.push_back(c);
}
