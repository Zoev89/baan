#ifndef IMAINWINDOWDRAWINGMOCK_H
#define IMAINWINDOWDRAWINGMOCK_H
#include "gmock/gmock.h"
#include "IMainWindowDrawing.h"
#include <string>

class IMainWindowDrawingMock: public IMainWindowDrawing
{
public:
    MOCK_METHOD1(SetBitmap, void(const std::string &));
    MOCK_METHOD0(RedrawAll, void());
    MOCK_METHOD4(RedrawRec, void(int, int ,int, int));
    MOCK_METHOD6(BlokDisplay, void(int, int ,int, int, const std::string&, int));
    MOCK_METHOD4(BlokClearEditModeDisplay, void(bool ,int, int, const std::string&));
    MOCK_METHOD3(DisplayString, void(int, int, const std::string&));
    MOCK_METHOD6(DrawLine, void(int, int ,int, int, int, BaanColor));
    MOCK_METHOD5(VierkantDisplay, void(int, int ,int, int, BaanColor));
};

#endif // IMAINWINDOWDRAWINGMOCK_H
