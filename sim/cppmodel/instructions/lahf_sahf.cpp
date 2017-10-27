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

// lahf
void EmulatorPimpl::lahf9f()
{
    auto flags = registers->get_flags();
    registers->set(AH, flags & 0xff);
}

// sahf
void EmulatorPimpl::sahf9e()
{
    auto flags = registers->get(AH);
    registers->set_flags(flags, 0xff);
}
