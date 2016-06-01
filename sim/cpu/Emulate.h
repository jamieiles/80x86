#ifndef __EMULATE_H__
#define __EMULATE_H__

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <memory>

#include "Memory.h"
#include "ModRM.h"

enum InterruptVectorOffset {
    VEC_DIVIDE_ERROR = 0,
    VEC_SINGLE_STEP = 4,
    VEC_NMI = 8,
    VEC_INT = 12,
    VEC_OVERFLOW = 16
};

static inline phys_addr get_phys_addr(uint16_t segment,
                                      uint32_t displacement)
{
    return ((static_cast<uint32_t>(segment) << 4) + displacement) % (1 * 1024 * 1024);
}

class EmulatorPimpl;
class Memory;
class RegisterFile;

class Emulator {
public:
    Emulator(RegisterFile *registers);
    virtual ~Emulator();
    size_t emulate();
    void set_memory(Memory *mem);
    void set_io(Memory *io);
    bool has_trapped() const;
    void reset();

private:
    std::unique_ptr<EmulatorPimpl> pimpl;
};

#endif /* __EMULATE_H__ */
