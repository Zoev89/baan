#ifndef TELEFOONCONNECTIEMOCK_H
#define TELEFOONCONNECTIEMOCK_H
#include "ITelefoonConnectie.h"
#include "gmock/gmock.h"

class ITelefoonConnectieMock : public ITelefoonConnectie
{
public:
    MOCK_METHOD0(Connect, void());
    MOCK_METHOD0(Disconnect, void());
    MOCK_METHOD3(SetActiveRegelaar, void(int, int, int));
};


#endif // TELEFOONCONNECTIEMOCK_H
