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

void EmulatorPimpl::movsba4()
{
    auto movs = [&]() {
        auto v = mem->read<uint8_t>(get_phys_addr(
            registers->get(get_segment(false)), registers->get(SI)));
        mem->write<uint8_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)), v);

        if (registers->get_flag(DF)) {
            registers->set(DI, registers->get(DI) - 1);
            registers->set(SI, registers->get(SI) - 1);
        } else {
            registers->set(DI, registers->get(DI) + 1);
            registers->set(SI, registers->get(SI) + 1);
        }
    };
    do_rep(movs, []() { return false; });
}

void EmulatorPimpl::movswa5()
{
    auto movs = [&]() {
        auto v = mem->read<uint16_t>(get_phys_addr(
            registers->get(get_segment(false)), registers->get(SI)));
        mem->write<uint16_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)), v);

        if (registers->get_flag(DF)) {
            registers->set(DI, registers->get(DI) - 2);
            registers->set(SI, registers->get(SI) - 2);
        } else {
            registers->set(DI, registers->get(DI) + 2);
            registers->set(SI, registers->get(SI) + 2);
        }
    };
    do_rep(movs, []() { return false; });
}
