#ifndef PROGRAMMAMOCK_H
#define PROGRAMMAMOCK_H
#include "IProgramma.h"
#include "gmock/gmock.h"

class IProgrammaMock: public IProgramma
{
public:

    MOCK_CONST_METHOD0(GetProgrammaNaam, boost::filesystem::path());
    MOCK_METHOD2(HerlaadProgramma,  void(const boost::filesystem::path& programmaNaam, int regelaarNummer));
};


#endif // PROGRAMMAMOCK_H
