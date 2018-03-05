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

`ifndef REGISTERENUM
`define REGISTERENUM

typedef enum bit [2:0] {
    AX,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI
} GPR16_t;

typedef enum bit [2:0] {
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH
} GPR8_t;

typedef enum bit [1:0] {
    ES,
    CS,
    SS,
    DS
} SR_t;

typedef enum bit [1:0] {
    REP_PREFIX_NONE,
    REP_PREFIX_E,
    REP_PREFIX_NE
} RepPrefix;

`endif
