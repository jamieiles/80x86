#pragma once

#include <cassert>
#include <stdexcept>

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "CPU.h"
#include "PIC.h"
#include <stdint.h>

class BadTimer : public std::runtime_error
{
public:
    explicit BadTimer(const char *what) : std::runtime_error(what)
    {
    }
};

#define assert_timer(expr) \
    if (!(expr))           \
        throw BadTimer("Invalid timer config: " #expr);

static const auto cycles_per_tick = 50000000 / (65536 * 18.2);

class TimerTick : public IOPorts
{
public:
    explicit TimerTick(PIC *pic)
        : IOPorts(0x0040, 2),
          pic(pic),
          access(0),
          operating_mode(0),
          count(0),
          latched(false),
          latched_val(0),
          access_low(false),
          reload(0)
    {
    }

    void write8(uint16_t port_num, unsigned offs, uint8_t v)
    {
        if (port_num == 2 && offs == 1)
            write_mode(v);
        else if (port_num == 0 && offs == 0)
            write_reload(v);
    }

    uint8_t read8(uint16_t port_num, unsigned offs)
    {
        if (port_num != 0 && offs != 0)
            return 0;

        auto counter_val = this->latched ? this->latched_val : read_counter();
        if (access_low) {
            access_low = false;
            return counter_val;
        } else {
            return counter_val >> 8;
        }
    }

    void tick(unsigned cycles)
    {
        if (count <= cycles) {
            if (operating_mode == 2) {
                pic->raise_irq(0);
                do_reload();
            }
        } else {
            count -= cycles;
        }
    }

private:
    PIC *pic;
    uint8_t access;
    uint8_t operating_mode;
    uint32_t count;
    bool latched;
    uint16_t latched_val;
    bool access_low;
    uint16_t reload;

    uint16_t read_counter() const
    {
        return count / cycles_per_tick;
    }

    void do_reload()
    {
        if (reload == 0)
            reload = 0xffff;

        count = static_cast<uint32_t>(reload) * cycles_per_tick;
    }

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & access;
        ar & operating_mode;
        ar & latched;
        ar & latched_val;
        ar & access_low;
        ar & reload;
        ar & count;
        // clang-format on
    }

    void write_mode(uint8_t v)
    {
        auto channel = v >> 6;
        auto access = (v >> 4) & 0x3;
        auto is_bcd = v & 0x1;

        // Ignore DRAM refresh and sound
        if (channel != 0)
            return;

        this->access = access;
        if (this->access != 0) {
            operating_mode = (v >> 1) & 0x7;
            assert_timer(operating_mode == 2);
        }
        assert_timer(is_bcd == 0);

        access_low = this->access != 2;
        if (this->access == 0) {
            this->latched_val = read_counter();
            this->latched = true;
        }
    }

    void write_reload(uint8_t v)
    {
        if (this->access_low) {
            reload &= ~0xff;
            reload |= v;
        } else {
            reload &= ~0xff00;
            reload |= static_cast<uint16_t>(v) << 8;
        }

        if (this->access == 0 && !this->access_low)
            do_reload();

        if (this->access == 0)
            this->access_low = false;
    }
};
