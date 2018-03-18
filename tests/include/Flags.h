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

#include <sstream>
#include <gtest/gtest.h>

static std::string flags_to_string(uint16_t flags)
{
    std::stringstream ss;

    if (flags & CF)
        ss << "CF ";
    if (flags & PF)
        ss << "PF ";
    if (flags & AF)
        ss << "AF ";
    if (flags & ZF)
        ss << "ZF ";
    if (flags & SF)
        ss << "SF ";
    if (flags & TF)
        ss << "TF ";
    if (flags & IF)
        ss << "IF ";
    if (flags & DF)
        ss << "DF ";
    if (flags & OF)
        ss << "OF ";

    return ss.str();
}

static inline ::testing::AssertionResult AssertFlagsEqual(const char *m_expr,
                                                          const char *n_expr,
                                                          uint16_t m,
                                                          uint16_t n)
{
    if (m == n)
        return ::testing::AssertionSuccess();

    auto m_repr = flags_to_string(m);
    auto n_repr = flags_to_string(n);

    return ::testing::AssertionFailure()
           << m_expr << " and " << n_expr << " are not equal "
           << "m(" << m_repr << ") "
           << "n(" << n_repr << ") " << std::hex << m << "/" << n;
}
