#ifndef IMAINSCREENCONTROLSMOCK_H
#define IMAINSCREENCONTROLSMOCK_H
#include "gmock/gmock.h"
#include "IMainScreenControls.h"

class IMainScreenControlsMock: public IMainScreenControls
{
public:
    MOCK_METHOD1(mainWindowLabel, void(const std::string& msg));
    MOCK_METHOD0(mainWindowOpenActivate, void());
    MOCK_METHOD0(mainWindowOpenDeactivate, void());
    MOCK_METHOD0(mainWindowCloseActivate, void());
    MOCK_METHOD0(mainWindowCloseDeactivate, void());
    MOCK_METHOD0(mainWindowTreinActivate, void());
    MOCK_METHOD0(mainWindowTreinDeactivate, void());
    MOCK_METHOD0(mainWindowRedraw, void());
};

#endif // IMAINSCREENCONTROLSMOCK_H
