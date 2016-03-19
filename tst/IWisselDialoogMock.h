#ifndef IWISSELDIALOOGMOCK_H
#define IWISSELDIALOOGMOCK_H
#include "gmock/gmock.h"
#include <string>
#include "IWisselDialoog.h"

class IWisselDialoogMock: public IWisselDialoog
{
public:
    MOCK_METHOD0(ClearAllFields, void());
    MOCK_METHOD0(RunDialogOk, bool());
    MOCK_METHOD1(SetAdres, void(float));
    MOCK_METHOD0(GetAdres, float());

    MOCK_METHOD1(SetUitleg, void(const std::string&));
    MOCK_METHOD0(GetUitleg, std::string());

    MOCK_METHOD1(SetHwAan, void(int));
    MOCK_METHOD0(GetHwAan, int());
    MOCK_METHOD1(SetHwAanToolTip, void(const std::string&));

    MOCK_METHOD1(SetHwTot, void(int));
    MOCK_METHOD0(GetHwTot, int());
    MOCK_METHOD1(SetHwTotToolTip, void(const std::string&));

    MOCK_METHOD1(SetTypeIndex, void(int));
    MOCK_METHOD0(GetTypeIndex, int());
    MOCK_METHOD1(SetTypeLabels, void(std::vector<std::string>));
    MOCK_METHOD0(GetTypeLabels, std::vector<std::string>());

    MOCK_METHOD1(SetAanTijd, void(int));
    MOCK_METHOD0(GetAanTijd, int());
    MOCK_METHOD1(SetAanTijdToolTip, void(const std::string&));

    MOCK_METHOD1(SetUitTijd, void(int));
    MOCK_METHOD0(GetUitTijd, int());
    MOCK_METHOD1(SetUitTijdToolTip, void(const std::string&));

    MOCK_METHOD1(SetHoogte, void(int));
    MOCK_METHOD0(GetHoogte, int());
    MOCK_METHOD1(SetHoogteToolTip, void(const std::string&));

    MOCK_METHOD1(SetBreedte, void(int));
    MOCK_METHOD0(GetBreedte, int());
    MOCK_METHOD1(SetBreedteToolTip, void(const std::string&));
    MOCK_METHOD0(GetBreedteToolTip, std::string());

    // wissels
    MOCK_METHOD2(SetAansluiting, void(int, const std::string&));
    MOCK_METHOD1(GetAansluiting, std::string(int));
    MOCK_METHOD2(SetAansluitingToolTip, void(int, const std::string&));

    MOCK_METHOD2(SetLengte, void(int, int));
    MOCK_METHOD1(GetLengte, int(int));
    MOCK_METHOD2(SetLengteToolTip, void(int, const std::string&));

    MOCK_METHOD2(SetMaxSnelheid, void(int, int));
    MOCK_METHOD1(GetMaxSnelheid, int(int));
    MOCK_METHOD2(SetMaxSnelheidToolTip, void(int, const std::string&));

    MOCK_METHOD1(SetDraaiAansluitingen, void(std::vector<DraaiSchijfAansluiting>));
    MOCK_METHOD0(GetDraaiAansluitingen, std::vector<DraaiSchijfAansluiting>());
};

#endif // IWISSELDIALOOGMOCK_H
