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

// in al, data8
void EmulatorPimpl::ine4()
{
    auto port_num = fetch_byte();

    registers->set(AL, read_io8(port_num));
}

// in ax, data8
void EmulatorPimpl::ine5()
{
    auto port_num = fetch_byte();

    registers->set(AX, read_io16(port_num));
}

// in al, dx
void EmulatorPimpl::inec()
{
    registers->set(AL, read_io8(registers->get(DX)));
}

// in ax, dx
void EmulatorPimpl::ined()
{
    registers->set(AX, read_io16(registers->get(DX)));
}

// out data8, al
void EmulatorPimpl::oute6()
{
    auto port_num = fetch_byte();

    write_io8(port_num, registers->get(AL));
}

// out data8, ax
void EmulatorPimpl::oute7()
{
    auto port_num = fetch_byte();

    write_io16(port_num, registers->get(AX));
}

// out dx, al
void EmulatorPimpl::outee()
{
    write_io8(registers->get(DX), registers->get(AL));
}

// out dx, ax
void EmulatorPimpl::outef()
{
    write_io16(registers->get(DX), registers->get(AX));
}
