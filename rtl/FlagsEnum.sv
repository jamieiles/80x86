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

`ifndef FLAGSENUM
`define FLAGSENUM

typedef enum bit [3:0] {
    CF_IDX = 4'd0,
    PF_IDX = 4'd2,
    AF_IDX = 4'd4,
    ZF_IDX = 4'd6,
    SF_IDX = 4'd7,
    TF_IDX = 4'd8,
    IF_IDX = 4'd9,
    DF_IDX = 4'd10,
    OF_IDX = 4'd11
} FLAG_t;

`endif // FLAGSENUM
