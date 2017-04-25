#pragma once
#include "CPU.h"
#include <cassert>
#include <deque>
#include <stdint.h>
#include <fstream>

// A dumb SPI mode SD card emulation.  Only supports basic block operations,
// standard capacity, no CRC checking or generation and no real errors, this
// is not a real SD card model!
//
// Blocklen is ignored and will always be 512 bytes.
// ACMD messages are ignored too.
class SPI : public IOPorts {
private:
    enum SPIState {
        STATE_IDLE,
        STATE_RECEIVING,
        STATE_TRANSMITTING
    };
public:
    SPI(const std::string &disk_image_path);
    void write8(uint16_t port_num, unsigned offs, uint8_t v);
    void write16(uint16_t port_num, uint16_t v);
    uint8_t read8(uint16_t port_num, unsigned offs);
    uint16_t read16(uint16_t __unused port_num);
private:
    void transfer(uint8_t mosi_val);
    bool transmit_ready();
    void read_block();
    uint16_t control_reg;
    uint8_t rx_val;
    SPIState state;
    std::vector<uint8_t> mosi_buf;
    std::deque<uint8_t> miso_buf;
    std::ifstream disk_image;
};
