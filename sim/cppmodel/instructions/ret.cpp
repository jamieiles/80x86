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

void EmulatorPimpl::retc3()
{
    auto ip = pop_word();

    registers->set(IP, ip);
    jump_taken = true;
}

void EmulatorPimpl::retc2()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();

    registers->set(IP, ip);
    registers->set(SP, registers->get(SP) + displacement);
    jump_taken = true;
}

void EmulatorPimpl::retcb()
{
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    jump_taken = true;
}

void EmulatorPimpl::retca()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set(SP, registers->get(SP) + displacement);
    jump_taken = true;
}

void EmulatorPimpl::iretcf()
{
    auto ip = pop_word();
    auto cs = pop_word();
    auto flags = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set_flags(flags);
    jump_taken = true;
}
