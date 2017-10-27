// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#define bda_write(cpu, field, val)                                            \
    ({                                                                        \
        __typeof__(((struct bios_data_area *)0)->field) _p = (val);           \
        cpu->get_memory()->template write<__typeof__(_p)>(                    \
            get_phys_addr(0x40, offsetof(struct bios_data_area, field)), _p); \
    })

#define bda_read(cpu, field)                                              \
    ({                                                                    \
        __typeof__(((struct bios_data_area *)0)->field) _p;               \
        _p = cpu->get_memory()->template read<__typeof__(_p)>(            \
            get_phys_addr(0x40, offsetof(struct bios_data_area, field))); \
        _p;                                                               \
    })
