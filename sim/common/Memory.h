#pragma once

#include <sys/types.h>
#include <stdint.h>

const size_t MEMORY_SIZE = 1 * 1024 * 1024;
typedef uint32_t phys_addr;

class Memory {
public:
    Memory();
    template <typename T>
        void write(phys_addr addr, T val);
    template <typename T>
        T read(phys_addr addr) const;
    bool has_written() const;
    void clear_has_written();
private:
    uint8_t mem[MEMORY_SIZE];
    bool written;
};

#ifndef MEM_INIT_BYTE_VAL
#define MEM_INIT_BYTE_VAL 0x00
#endif

constexpr uint8_t mem_init_8 = MEM_INIT_BYTE_VAL;
constexpr uint16_t mem_init_16 = (static_cast<uint16_t>(MEM_INIT_BYTE_VAL) << 8) |
    static_cast<uint16_t>(MEM_INIT_BYTE_VAL);
