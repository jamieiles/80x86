#include <string>
#include <vector>

#include <gtest/gtest.h>

template <typename T1, typename T2>
struct DivTest {
    T1 v1;
    T2 v2;
    T2 quotient;
    T2 remainder;
};

using Div8Params = struct DivTest<uint16_t, uint8_t>;
using Div16Params = struct DivTest<uint32_t, uint16_t>;
using IDiv8Params = struct DivTest<int16_t, int8_t>;
using IDiv16Params = struct DivTest<int32_t, int16_t>;

template <typename T>
struct SignMagnitude {
    bool is_negative;
    T magnitude;
};

template <typename UT, typename ST>
SignMagnitude<UT> twos_comp_to_sm(UT v)
{
    SignMagnitude<UT> r;

    r.is_negative = v < 0;

    const ST mask = static_cast<ST>(v) >> ((sizeof(ST) * 8) - 1);
    r.magnitude = (static_cast<UT>(v) + mask) ^ mask;

    return r;
}

// Non-restoring division
std::tuple<uint16_t, uint16_t> divide(uint32_t dividend,
                                      uint16_t divisor,
                                      bool is_signed=false)
{
    int64_t P;
    uint32_t D;
    SignMagnitude<uint32_t> dividend_sm =
        twos_comp_to_sm<uint32_t, int32_t>(dividend);
    SignMagnitude<uint16_t> divisor_sm =
        twos_comp_to_sm<uint16_t, int16_t>(divisor);

    if (!is_signed) {
        P = static_cast<int64_t>(dividend);
        D = static_cast<uint32_t>(divisor) << 16;
    } else {
        P = static_cast<int64_t>(dividend_sm.magnitude);
        D = static_cast<uint32_t>(divisor_sm.magnitude) << 16;
    }

    uint16_t q = 0;

    for (int i = 15; i >= 0; --i) {
        if (P >= 0) {
            q |= (1 << i);
            P = (2 * P) - D;
        } else {
            P = (2 * P) + D;
        }
    }

    q -= ~q;

    if (P < 0) {
        --q;
        P += D;
    }

    return std::tuple<uint16_t, uint16_t>(q, P >> 16);
}

// Non-restoring division
std::tuple<int16_t, int16_t> idivide(int32_t dividend,
                                     int16_t divisor)
{
    int16_t quotient, remainder;
    std::tie(quotient, remainder) = divide(static_cast<uint32_t>(dividend),
                                           static_cast<uint16_t>(divisor),
                                           true);

    if ((dividend < 0 && divisor >= 0) || (dividend >= 0 && divisor < 0))
        quotient = -quotient;
    if (dividend < 0)
        remainder = -remainder;

    return std::tuple<int16_t, int16_t>(quotient, remainder);
}

class DivisionAlg16 : public ::testing::TestWithParam<Div16Params> {
};
TEST_P(DivisionAlg16, Result)
{
    uint16_t quotient, remainder;

    std::tie(quotient, remainder) = divide(GetParam().v1,
                                           GetParam().v2);
    EXPECT_EQ(quotient, GetParam().quotient);
    EXPECT_EQ(remainder, GetParam().remainder);
}
INSTANTIATE_TEST_CASE_P(Div, DivisionAlg16,
    ::testing::Values(
    Div16Params{ 100, 20, 5, 0 },
    Div16Params{ 500, 250, 2, 0 },
    Div16Params{ 10, 3, 3, 1 },
    Div16Params{ 128000, 10, 12800, 0 },
    Div16Params{ 130000, 65000, 2, 0 },
    Div16Params{ 0x109, 0xe90b, 0, 265 }
));

class IDivisionAlg16 : public ::testing::TestWithParam<IDiv16Params> {
};
TEST_P(IDivisionAlg16, Result)
{
    int16_t quotient, remainder;

    std::tie(quotient, remainder) = idivide(GetParam().v1,
                                            GetParam().v2);
    EXPECT_EQ(quotient, GetParam().quotient);
    EXPECT_EQ(remainder, GetParam().remainder);
}
INSTANTIATE_TEST_CASE_P(IDiv, IDivisionAlg16,
    ::testing::Values(
    IDiv16Params{ 10, 3, 3, 1 },
    IDiv16Params{ 10, -3, -3, 1 },
    IDiv16Params{ -10, -3, 3, -1 },
    IDiv16Params{ -10, 3, -3, -1 }
));
