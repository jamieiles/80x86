#include "SPI.h"
#include "CPU.h"

#include <iostream>
#include <iomanip>
#include <stdint.h>

SPI::SPI(const std::string &disk_image_path)
    : IOPorts(0xfff0, 2),
    control_reg(0),
    rx_val(0),
    state(STATE_IDLE),
    disk_image(disk_image_path,
               std::ios::binary | std::ios::in | std::ios::out),
    do_write(false)
{}

void SPI::write8(uint16_t port_num, unsigned offs, uint8_t v)
{
    auto shift = 8 * offs;
    uint16_t mask = 0xff << shift;

    if (port_num == 0) {
        control_reg &= ~mask;
        control_reg |= static_cast<uint16_t>(v) << shift;
    } else {
        if (offs == 0)
            transfer(v);
    }
}

void SPI::write16(uint16_t port_num, uint16_t v)
{
    write8(port_num, 0, v);
    write8(port_num, 1, v >> 8);
}

uint8_t SPI::read8(uint16_t port_num, unsigned offs)
{
    auto shift = 8 * offs;

    if (port_num == 0)
        return control_reg >> shift;
    else
        return offs == 0 ? rx_val : 0x00;
}

uint16_t SPI::read16(uint16_t __unused port_num)
{
    return read8(port_num, 0) |
        (static_cast<uint16_t>(read8(port_num, 1)) << 8);
}

void SPI::transfer(uint8_t mosi_val)
{
    switch (state) {
    case STATE_IDLE:
        do_write = false;
        if (mosi_val != 0xff && !(control_reg & (1 << 9))) {
            state = STATE_RECEIVING;
            mosi_buf.push_back(mosi_val);
        }
        rx_val = 0xff;
        break;
    case STATE_RECEIVING:
        mosi_buf.push_back(mosi_val);
        if (transmit_ready())
            state = STATE_TRANSMITTING;
        rx_val = 0xff;
        break;
    case STATE_TRANSMITTING:
        if (miso_buf.size() == 0) {
            rx_val = 0xff;
            state = do_write ? STATE_WAIT_FOR_DATA : STATE_IDLE;
            mosi_buf.clear();
        } else {
            rx_val = miso_buf[0];
            miso_buf.pop_front();
        }
        break;
    case STATE_WAIT_FOR_DATA:
        rx_val = 0xff;
        if (mosi_val == 0xfe)
            state = STATE_DO_WRITE_BLOCK;
        break;
    case STATE_DO_WRITE_BLOCK:
        if (write_count < 512) {
            disk_image.put(mosi_val);
            rx_val = 0xff;
        } else if (write_count < 514) {
            // CRC
            rx_val = 0xff;
        } else {
            rx_val = 0x5; // Received OK.
            state = STATE_IDLE;
        }
        ++write_count;
        break;
    };
}

bool SPI::transmit_ready()
{
    assert(mosi_buf.size() > 0);

    switch (mosi_buf[0]) {
    case 0x77: // ACMD
    case 0x40: // Reset, fallthrough
    case 0x48: // IF COND
    case 0x50: // Set blocklen
        if (mosi_buf.size() >= 7) {
            miso_buf = { 0x01 };
            return true;
        }
        break;
    case 0x69: // CMD41, reset
        if (mosi_buf.size() >= 7) {
            miso_buf = { 0x00 };
            return true;
        }
        break;
    case 0x51: // Read block
        if (mosi_buf.size() >= 7) {
            read_block();
            return true;
        }
        break;
    case 0x58: // Write block
        if (mosi_buf.size() >= 7) {
            write_block();
            return true;
        }
        break;
    case 0x7a: // OCR
        if (mosi_buf.size() >= 7) {
            miso_buf = { 0x01, 0x00, 0x00, 0x00, 0x00 };
            return true;
        }
        break;
    default:
        std::cout << "unknown command " << std::hex << (unsigned)mosi_buf[0] << std::endl;
        // Generic error
        miso_buf = { 0xfe };
        return true;
    }

    return false;
}

void SPI::read_block()
{
    union {
        uint8_t u8[4];
        uint32_t u32;
    } converter = {
        .u8 = {
            mosi_buf[4], mosi_buf[3], mosi_buf[2], mosi_buf[1]
        },
    };
    // Padding, R1, padding, start of data
    miso_buf = { 0xff, 0xff, 0x00, 0xff, 0xfe  };
    // Data
    disk_image.seekg(converter.u32, std::ios::beg);
    for (auto m = 0; m < 512; ++m)
        miso_buf.push_back(disk_image.get());
    // CRC
    for (auto m = 0; m < 2; ++m)
        miso_buf.push_back(0x77);
}

void SPI::write_block()
{
    union {
        uint8_t u8[4];
        uint32_t u32;
    } converter = {
        .u8 = {
            mosi_buf[4], mosi_buf[3], mosi_buf[2], mosi_buf[1]
        },
    };

    // Padding, R1, padding, Accepted
    miso_buf = { 0xff, 0x00, 0xff, 0xff, 0x05, };
    do_write = true;
    write_count = 0;
    disk_image.seekg(converter.u32, std::ios::beg);
}
