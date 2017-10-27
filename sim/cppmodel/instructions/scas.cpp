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

void EmulatorPimpl::scasbae()
{
    auto scas = [&]() {
        auto v = mem->read<uint8_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)));

        uint8_t result;
        uint16_t flags;
        std::tie(flags, result) = do_sub<uint8_t>(registers->get(AL), v);
        registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 1);
        else
            registers->set(DI, registers->get(DI) + 1);
    };
    do_rep(scas, [&]() { return string_rep_complete(); });
}

void EmulatorPimpl::scasbaf()
{
    auto scas = [&]() {
        auto v = mem->read<uint16_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)));

        uint16_t result;
        uint16_t flags;
        std::tie(flags, result) = do_sub<uint16_t>(registers->get(AX), v);
        registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 2);
        else
            registers->set(DI, registers->get(DI) + 2);
    };
    do_rep(scas, [&]() { return string_rep_complete(); });
}
