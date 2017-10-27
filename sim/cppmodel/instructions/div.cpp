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

// div r/m, 8-bit
void EmulatorPimpl::divf6()
{
    auto divisor = read_data<uint8_t>();
    auto dividend = registers->get(AX);

    if (divisor == 0 || ((dividend / divisor) & 0xff00)) {
        auto flags = registers->get_flags();
        push_word(flags);
        push_word(registers->get(CS));
        push_word(registers->get(IP));

        flags &= ~(IF | TF);
        registers->set_flags(flags, IF | TF);

        auto new_cs = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 2);
        auto new_ip = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 0);

        registers->set(CS, new_cs);
        registers->set(IP, new_ip);
        jump_taken = true;

        return;
    }

    uint8_t quotient = dividend / divisor;
    uint8_t remainder = dividend % divisor;

    registers->set(AH, remainder);
    registers->set(AL, quotient);
}

// div r/m, 16-bit
void EmulatorPimpl::divf7()
{
    auto divisor = read_data<uint16_t>();
    uint32_t dividend =
        (static_cast<uint32_t>(registers->get(DX)) << 16) | registers->get(AX);

    if (divisor == 0 || ((dividend / divisor) & 0xffff0000)) {
        auto flags = registers->get_flags();
        push_word(flags);
        push_word(registers->get(CS));
        push_word(registers->get(IP));

        flags &= ~(IF | TF);
        registers->set_flags(flags, IF | TF);

        auto new_cs = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 2);
        auto new_ip = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 0);

        registers->set(CS, new_cs);
        registers->set(IP, new_ip);
        jump_taken = true;

        return;
    }

    uint16_t quotient = dividend / divisor;
    uint16_t remainder = dividend % divisor;

    registers->set(DX, remainder);
    registers->set(AX, quotient);
}

// idiv r/m, 8-bit
void EmulatorPimpl::idivf6()
{
    int8_t divisor = static_cast<int8_t>(read_data<uint8_t>());
    int16_t dividend = static_cast<int16_t>(registers->get(AX));

    if (divisor == 0 || (dividend / divisor < -128) ||
        (dividend / divisor > 127)) {
        auto flags = registers->get_flags();
        push_word(flags);
        push_word(registers->get(CS));
        push_word(registers->get(IP));

        flags &= ~(IF | TF);
        registers->set_flags(flags, IF | TF);

        auto new_cs = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 2);
        auto new_ip = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 0);

        registers->set(CS, new_cs);
        registers->set(IP, new_ip);
        jump_taken = true;

        return;
    }

    int8_t quotient = dividend / divisor;
    int8_t remainder = dividend % divisor;

    registers->set(AH, remainder);
    registers->set(AL, quotient);
}

// div r/m, 16-bit
void EmulatorPimpl::idivf7()
{
    auto divisor = static_cast<int16_t>(read_data<uint16_t>());
    int32_t dividend =
        (static_cast<uint32_t>(registers->get(DX)) << 16) | registers->get(AX);

    if (divisor == 0 || (dividend / divisor < -32768) ||
        (dividend / divisor) > 32767) {
        auto flags = registers->get_flags();
        push_word(flags);
        push_word(registers->get(CS));
        push_word(registers->get(IP));

        flags &= ~(IF | TF);
        registers->set_flags(flags, IF | TF);

        auto new_cs = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 2);
        auto new_ip = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 0);

        registers->set(CS, new_cs);
        registers->set(IP, new_ip);
        jump_taken = true;

        return;
    }

    int16_t quotient = dividend / divisor;
    int16_t remainder = dividend % divisor;

    registers->set(DX, remainder);
    registers->set(AX, quotient);
}
