#ifndef __MEMORY_H__
#define __MEMORY_H__

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
private:
    uint8_t mem[MEMORY_SIZE];
};

#endif /* __MEMORY_H__ */
