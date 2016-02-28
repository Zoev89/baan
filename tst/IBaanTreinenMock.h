#ifndef IBAANTREINENMOCK_H
#define IBAANTREINENMOCK_H
#include "IBaanTreinen.h"
#include "gmock/gmock.h"

class IBaanTreinenMock : public IBaanTreinen
{
public:
    MOCK_METHOD4(baanCreateTreinen, void(int, int, int, int));
    MOCK_METHOD4(baanAddTrein, void(char *, char *, int, const std::string&));
    MOCK_METHOD0(baanDestroyTreinen, void());
    MOCK_METHOD1(baanVerwijderTrein, void(int));
    MOCK_METHOD1(baanTreinenActivateRegelaar, void(int));
};

#endif // IBAANTREINENMOCK_H
