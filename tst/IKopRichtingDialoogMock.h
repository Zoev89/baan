#ifndef IKOPRICHTINGDIALOOGMOCK_H
#define IKOPRICHTINGDIALOOGMOCK_H
#include "IKopRichtingDialoog.h"
#include "gmock/gmock.h"

class IKopRichtingDialoogMock : public IKopRichtingDialoog
{
public:
    MOCK_METHOD0(RunDialogOk, bool());

    MOCK_METHOD1(SetKopBlok, void(int));
    MOCK_METHOD0(GetKopBlok, int());
    MOCK_METHOD1(SetLengte, void(int));
    MOCK_METHOD0(GetLengte, int());
    MOCK_METHOD1(SetKopBlokMee, void(bool));
    MOCK_METHOD0(GetKopBlokMee, bool());
    MOCK_METHOD1(SetVooruitTerug, void(bool));
    MOCK_METHOD0(GetVoorUitTerug, bool());
    MOCK_METHOD1(SetErrorText, void(const std::string& ));
    MOCK_METHOD0(GetErrorText, std::string());
};

#endif // IKOPRICHTINGDIALOOGMOCK_H
