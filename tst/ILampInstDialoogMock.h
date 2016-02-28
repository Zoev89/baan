#ifndef ILAMPINSTDIALOOGMOCK_H
#define ILAMPINSTDIALOOGMOCK_H
#include "ILampInstDialoog.h"
#include "gmock/gmock.h"

class ILampInstDialoogMock : public ILampInstDialoog
{
public:
    MOCK_METHOD0(RunDialogOk, bool());
    MOCK_METHOD1(SetAanTijd, void(int));
    MOCK_METHOD0(GetAanTijd, int());
    MOCK_METHOD1(SetUitTijd, void(int));
    MOCK_METHOD0(GetUitTijd, int());
};

#endif // ILAMPINSTDIALOOGMOCK_H
