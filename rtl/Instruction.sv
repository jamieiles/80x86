// Copyright Jamie Iles, 2018
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

typedef struct packed {
    logic invalid;
    logic [3:0] length;
    logic lock;
    RepPrefix rep;
    SR_t segment;
    logic has_segment_override;
    logic has_modrm;
    logic [1:0][15:0] immediates;
    logic [15:0] displacement;
    logic [7:0] mod_rm;
    logic [7:0] opcode;
} Instruction;
