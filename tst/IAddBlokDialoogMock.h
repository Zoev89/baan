#ifndef IADDBLOKDIALOOGMOCK_H
#define IADDBLOKDIALOOGMOCK_H
#include "IAddBlokDialoog.h"
#include "gmock/gmock.h"

class IAddBlokDialoogMock: public IAddBlokDialoog
{
public:
    MOCK_METHOD0(RunDialogOk, bool());
    MOCK_METHOD1(SetVrijBlok, void(int));
    MOCK_METHOD0(GetVrijBlok, int());
};


#endif // IADDBLOKDIALOOGMOCK_H
