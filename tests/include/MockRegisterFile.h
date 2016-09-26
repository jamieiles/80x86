#include <gmock/gmock.h>
#include "RegisterFile.h"

class MockRegisterFile : public RegisterFile {
public:
    MockRegisterFile()
        : RegisterFile()
    {}
    MOCK_METHOD0(reset, void());
    MOCK_METHOD2(set, void(GPR, uint16_t));
    MOCK_CONST_METHOD1(get, uint16_t(GPR));
    MOCK_CONST_METHOD0(get_flags, uint16_t());
    MOCK_METHOD1(set_flags, void(uint16_t));
    MOCK_CONST_METHOD0(has_written, bool());
    MOCK_METHOD0(clear_has_written, void());
};
