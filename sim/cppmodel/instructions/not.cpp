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

// not byte r/m
void EmulatorPimpl::notf6()
{
    auto v = read_data<uint8_t>();
    write_data<uint8_t>(~v);
}

// not word r/m
void EmulatorPimpl::notf7()
{
    auto v = read_data<uint16_t>();
    write_data<uint16_t>(~v);
}
