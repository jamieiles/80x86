#pragma once

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "CPU.h"
#include <stdint.h>

class TimerTick : public IOPorts
{
public:
    TimerTick(CPU *cpu)
        : IOPorts(0xffee, 1), cpu(cpu), ms_count(0), count(0), enabled(false)
    {
    }

    void write8(uint16_t __unused port_num,
                unsigned __unused offs,
                uint8_t __unused v)
    {
    }
    void write16(uint16_t __unused port_num, uint16_t v)
    {
        reload = v & 0x7fff;
        enabled = !!(v & (1 << 15));

        do_reload();
    }

    uint8_t read8(uint16_t __unused port_num, unsigned __unused offs)
    {
        return 0;
    }

    uint16_t read16(uint16_t __unused port_num)
    {
        return ms_count | (enabled << 15);
    }

    void tick(unsigned microseconds)
    {
        if (!enabled)
            return;

        if (count <= microseconds) {
            count = 0;
            cpu->raise_irq(0);
            do_reload();
        } else {
            count -= microseconds;
        }
    }

private:
    void do_reload()
    {
        ms_count = reload;
        count = ms_count * 1000;
    }

    CPU *cpu;
    uint16_t ms_count;
    uint32_t count;
    bool enabled;
    uint16_t reload;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int __unused version)
    {
        // clang-format off
        ar & ms_count;
        ar & count;
        ar & enabled;
        ar & reload;
        // clang-format on
    }
};
