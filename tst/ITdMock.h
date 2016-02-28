#ifndef ITDMOCK_H
#define ITDMOCK_H
#include "ITd.h"
#include "gmock/gmock.h"
class ITdMock : public ITd
{
public:
    MOCK_METHOD1(tdCreate, int(const std::string& string));
    MOCK_METHOD1(tdStart, void(int));
    MOCK_METHOD1(tdStop, void(int));
    MOCK_METHOD1(tdValueString, int(const std::string& string));
    MOCK_METHOD2(tdValue, void(int,int));
    MOCK_METHOD0(tdDump, void());
};

#endif // ITDMOCK_H
