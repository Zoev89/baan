#ifndef IBLOKINSTMOCK_H
#define IBLOKINSTMOCK_H
#include "gmock/gmock.h"
#include "IBlokInst.h"
class IBlokInstMock: public IBlokInst
{
public:
    MOCK_METHOD0(RunDialogOk, bool());
    MOCK_METHOD1(SetBlok, void(int));
    MOCK_METHOD0(GetBlok, int());
    MOCK_METHOD1(SetVolgendBlok, void(const std::string& blok));
    MOCK_METHOD0(GetVolgendBlok, std::string());
    MOCK_METHOD1(SetMaxSnelheid, void(int));
    MOCK_METHOD0(GetMaxSnelheid, int());
    MOCK_METHOD1(SetLengte, void(int));
    MOCK_METHOD0(GetLengte, int());
    MOCK_METHOD1(SetBovenLeiding, void(bool));
    MOCK_METHOD0(GetBovenLeiding, bool());
    MOCK_METHOD1(SetSein, void(int));
    MOCK_METHOD0(GetSein, int());
    MOCK_METHOD1(SetVoorKeurRichting, void(int));
    MOCK_METHOD0(GetVoorKeurRichting, int());
};

#endif // IBLOKINSTMOCK_H
