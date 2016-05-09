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

static inline ::testing::AssertionResult
AssertFlagsEqual(const char *m_expr, const char *n_expr, uint16_t m,
                 uint16_t n)
{
    if (m == n)
        return ::testing::AssertionSuccess();

    auto m_repr = flags_to_string(m);
    auto n_repr = flags_to_string(n);

    return ::testing::AssertionFailure()
        << m_expr << " and " << n_expr << " are not equal "
        << "m(" << m_repr << ") " << "n(" << n_repr << ")";
}

