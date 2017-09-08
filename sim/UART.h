#pragma once

#include <deque>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdexcept>

#include "UART.h"
#include "CPU.h"

class RawTTY
{
public:
    RawTTY()
    {
        if (tcgetattr(STDIN_FILENO, &old_termios))
            throw std::runtime_error("Failed to get termios");

        auto termios = old_termios;
        cfmakeraw(&termios);
        termios.c_cc[VMIN] = 0;
        termios.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &termios))
            throw std::runtime_error("Failed to set new termios");
    }

    ~RawTTY()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    }

private:
    struct termios old_termios;
};

class UART : public IOPorts
{
public:
    UART();
    void write8(uint16_t port_num, unsigned offs, uint8_t v);
    void write16(uint16_t port_num, uint16_t v);
    uint8_t read8(uint16_t port_num, unsigned offs);
    uint16_t read16(uint16_t port_num);
    void add_char(int c);

private:
    RawTTY raw_tty;
    std::deque<uint8_t> charbuf;
};
