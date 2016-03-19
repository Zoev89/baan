#ifndef INIEUWIODIALOOGMOCK_H
#define INIEUWIODIALOOGMOCK_H
#include "INieuwIODialoog.h"
#include "gmock/gmock.h"
class INieuwIODialoogMock : public INieuwIODialoog
{
public:
    MOCK_METHOD0(RunDialogOk, bool());
    MOCK_METHOD1(SetAdres, void(float));
    MOCK_METHOD0(GetAdres, float());
    MOCK_METHOD1(SetBlok, void(int));
    MOCK_METHOD0(GetBlok, int());
    MOCK_METHOD1(SetTypeIndex, void(int));
    MOCK_METHOD0(GetTypeIndex, int());
    MOCK_METHOD1(SetTypeLabels, void(std::vector<std::string> ));
};
#endif // INIEUWIODIALOOGMOCK_H
