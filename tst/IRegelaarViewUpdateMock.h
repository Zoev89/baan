#ifndef IREGELAARVIEWUPDATEMOCK_H
#define IREGELAARVIEWUPDATEMOCK_H
#include "IRegelaarViewUpdates.h"
#include "gmock/gmock.h"

class IRegelaarViewUpdatesMock: public IRegelaarViewUpdates
{
    MOCK_METHOD2(NieuweRegelaar, void(int, const std::string&));
    MOCK_METHOD2(SnelheidProgress, void(int, int));
    MOCK_METHOD2(Meldingen, void(int, const std::string&));
    MOCK_METHOD2(NewSnelheid, void(int, int));
    MOCK_METHOD2(RichtingVerandering, void(int, int));
    MOCK_METHOD2(IgnoreStop, void(int, bool));
    MOCK_METHOD2(ShowRegelaar, void(int, bool));
};

#endif // IREGELAARVIEWUPDATEMOCK_H
