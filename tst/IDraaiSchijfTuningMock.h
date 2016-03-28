#ifndef IDRAAISCHIJFTUNINGMOCK_H
#define IDRAAISCHIJFTUNINGMOCK_H

#include "IDraaiSchijfTuning.h"
#include "gmock/gmock.h"

class IDraaiSchijfTuningMock: public IDraaiSchijfTuning
{
public:
    MOCK_METHOD1(show, void(int basisAdres));
};


#endif // IDRAAISCHIJFTUNINGMOCK_H
