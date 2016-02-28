#ifndef ITREINENDIALOOGMOCK_H
#define ITREINENDIALOOGMOCK_H

#include "gmock/gmock.h"
#include "ITreinenDialoog.h"
#include <string>

class ITreinenDialoogMock : public ITreinenDialoog
{
public:
    MOCK_METHOD4(CreateTreinen, void(int, int ,int, int));
    MOCK_METHOD4(AddTrein, void(const std::string&, const std::string&, int ,const std::string&));
    MOCK_METHOD1(VerwijderTrein, void(int));
    MOCK_METHOD0(DestroyTreinen, void());
};

#endif // ITREINENDIALOOGMOCK_H
