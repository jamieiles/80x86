#pragma once

#define bda_write(cpu, field, val) ({ \
    __typeof__ (((struct bios_data_area *)0)->field) _p = (val); \
    cpu->get_memory()->template write<__typeof__(_p)>( \
        get_phys_addr(0x40, offsetof(struct bios_data_area, field)), _p); \
})

#define bda_read(cpu, field) ({ \
    __typeof__ (((struct bios_data_area *)0)->field) _p; \
    _p = cpu->get_memory()->template read<__typeof__(_p)>( \
        get_phys_addr(0x40, offsetof(struct bios_data_area, field))); \
    _p; \
})
