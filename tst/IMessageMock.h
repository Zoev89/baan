#ifndef IMESSAGEMOCK_H
#define IMESSAGEMOCK_H
#include "gmock/gmock.h"
#include "IMessage.h"

class IMessageMock : public IMessage
{
public:
    MOCK_METHOD1(message, void(const std::string str));
};

#endif // IMESSAGEMOCK_H
