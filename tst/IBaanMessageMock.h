#ifndef IBAANMESSAGEMOCK_H
#define IBAANMESSAGEMOCK_H
#include "IBaanMessage.h"
#include "gmock/gmock.h"

class IBaanMessageMock : public IBaanMessage
{
public:
    MOCK_METHOD4(Post, int(int, int, int, int));
    MOCK_METHOD0(Start, int());
    MOCK_METHOD0(Stop, void());
};

#endif // IBAANMESSAGEMOCK_H
