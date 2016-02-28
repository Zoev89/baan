#ifndef IVIEWMOCK_H
#define IVIEWMOCK_H
#include "IView.h"
#include "gmock/gmock.h"

class IViewMock : public IView
{
public:
    MOCK_METHOD0(Draw, void());
    MOCK_METHOD4(HandleMouseEvents, bool(MouseEventType, MouseButtonType, int, int));
    MOCK_METHOD1(HandleKeyBoardEvent, bool(KeyBoardEventType));
};

#endif // IVIEWMOCK_H
