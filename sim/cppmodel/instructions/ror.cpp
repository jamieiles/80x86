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

template <typename T>
std::pair<uint16_t, T> do_ror(T v, int count)
{
    uint16_t flags = 0;

    for (int i = 0; i < count; ++i) {
        flags &= ~CF;
        if (v & 0x1)
            flags |= CF;
        T lsb = v & 1;
        v >>= 1;
        v += (lsb << ((sizeof(T) * 8) - 1));
    }

    // 2 MSB's XOR'd == OF
    T msbs = (v >> ((sizeof(T) * 8) - 2)) & 0x3;
    if (msbs == 2 || msbs == 1)
        flags |= OF;

    return std::make_pair(flags, v);
}

// ror r/m, N
void EmulatorPimpl::rorc0()
{
    auto v = read_data<uint8_t>();
    auto count = fetch_byte();
    uint16_t flags;

    if (!(count & 0x1f))
        return;

    std::tie(flags, v) = do_ror(v, count & 0x1f);

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF);
}

// ror r/m, 1
void EmulatorPimpl::rorc1()
{
    auto v = read_data<uint16_t>();
    auto count = fetch_byte();
    uint16_t flags;

    if (!(count & 0x1f))
        return;

    std::tie(flags, v) = do_ror(v, count & 0x1f);

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF);
}

// ror r/m, 1
void EmulatorPimpl::rord0()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    std::tie(flags, v) = do_ror(v, 1);

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF | OF);
}

// ror r/m, 1
void EmulatorPimpl::rord1()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    std::tie(flags, v) = do_ror(v, 1);

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF | OF);
}

// ror r/m, N
void EmulatorPimpl::rord2()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    if (!registers->get(CL))
        return;

    std::tie(flags, v) = do_ror(v, registers->get(CL));

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF);
}

// ror r/m, N
void EmulatorPimpl::rord3()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    if (!registers->get(CL))
        return;

    std::tie(flags, v) = do_ror(v, registers->get(CL));

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF);
}
