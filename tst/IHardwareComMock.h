#ifndef IHARDWARECOMMOCK_H
#define IHARDWARECOMMOCK_H
#include "IHardwareCom.h"
#include "gmock/gmock.h"

class IHardwareComMock: public IHardwareCom
{
public:
    MOCK_METHOD1(nieuwItem, int(const hardwareArray_t & data));
    MOCK_METHOD1(krijgItem, int(hardwareArray_t & data));
    MOCK_METHOD0(aantalItems, int());
};

#endif // IHARDWARECOMMOCK_H
