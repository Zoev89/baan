#ifndef IREGELAARINSTELLINGENDIALOOGMOCK_H
#define IREGELAARINSTELLINGENDIALOOGMOCK_H
#include "IRegelaarInstellingenDialoog.h"
#include "gmock/gmock.h"

class IRegelaarInstellingenDialoogMock : public IRegelaarInstellingenDialoog
{
public:
    MOCK_METHOD0(RunDialogOk, bool());
    MOCK_METHOD1(SetLaatsteWagonCheck, void(bool));
    MOCK_METHOD0(GetLaatsteWagonCheck, bool());
    MOCK_METHOD1(SetEloc, void(bool));
    MOCK_METHOD0(GetEloc, bool());
    MOCK_METHOD1(SetMinSnelheid, void(int));
    MOCK_METHOD0(GetMinSnelheid, int());
    MOCK_METHOD1(SetMaxSnelheid, void(int));
    MOCK_METHOD0(GetMaxSnelheid, int());
    MOCK_METHOD1(SetTopSnelheid, void(int));
    MOCK_METHOD0(GetTopSnelheid, int());
    MOCK_METHOD1(SetTotaalAfstand, void(int));
    MOCK_METHOD0(GetTotaalAfstand, int());
    MOCK_METHOD1(SetTotaalTijd, void(const std::string& ));
    MOCK_METHOD0(GetTotaalTijd, std::string());
    MOCK_METHOD1(SetLengte, void(int));
    MOCK_METHOD0(GetLengte, int());
    MOCK_METHOD1(SetAlphaRijden, void(float));
    MOCK_METHOD0(GetAlphaRijden, float());
    MOCK_METHOD1(SetClipRijden, void(int));
    MOCK_METHOD0(GetClipRijden, int());
    MOCK_METHOD1(SetAlphaStoppen, void(float));
    MOCK_METHOD0(GetAlphaStoppen, float());
    MOCK_METHOD1(SetClipStoppen, void(int));
    MOCK_METHOD0(GetClipStoppen, int());
    MOCK_METHOD1(SetStand1, void(int));
    MOCK_METHOD0(GetStand1, int());
    MOCK_METHOD1(SetAfstand1, void(int));
    MOCK_METHOD0(GetAfstand1, int());
    MOCK_METHOD1(SetStand2, void(int));
    MOCK_METHOD0(GetStand2, int());
    MOCK_METHOD1(SetAfstand2, void(int));
    MOCK_METHOD0(GetAfstand2, int());
    MOCK_METHOD1(SetErrors, void(const std::string& ));
    MOCK_METHOD0(GetErrors, std::string());
    MOCK_METHOD1(SetKLpf, void(float));
    MOCK_METHOD0(GetKLpf, float());
    MOCK_METHOD1(SetPlusMinus, void(float));
    MOCK_METHOD0(GetPlusMinus, float());
    MOCK_METHOD1(SetHelling, void(float));
    MOCK_METHOD0(GetHelling, float());
    MOCK_METHOD1(SetDodeTijd, void(float));
    MOCK_METHOD0(GetDodeTijd, float());
    MOCK_METHOD1(SetProgrammaNaam, void(const std::string& ));
    MOCK_METHOD0(GetProgrammaNaam, std::string());
    MOCK_METHOD1(SetHerlaadProgramma, void(bool));
    MOCK_METHOD0(GetHerlaadProgramma, bool());
    MOCK_METHOD1(SetLangzaam, void(int));
    MOCK_METHOD0(GetLangzaam, int());
    MOCK_METHOD1(SetRijden, void(int));
    MOCK_METHOD0(GetRijden, int());
    MOCK_METHOD1(SetLocType, void(const std::string& ));
    MOCK_METHOD0(GetLocType, std::string());
    MOCK_METHOD1(SetLastStand1, void(int));
    MOCK_METHOD0(GetLastStand1, int());
    MOCK_METHOD1(SetLastGain1, void(float));
    MOCK_METHOD0(GetLastGain1, float());
    MOCK_METHOD1(SetLastStand2, void(int));
    MOCK_METHOD0(GetLastStand2, int());
    MOCK_METHOD1(SetLastGain2, void(float));
    MOCK_METHOD0(GetLastGain2, float());
    MOCK_METHOD1(SetLastRegelKeuze, void(int));
    MOCK_METHOD0(GetLastRegelKeuze, int());
    MOCK_METHOD1(SetStandUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetAlphaUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetClipUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetKlpfUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetPlusMinusUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetHellingUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetDodeTijdUitleg, void(const std::string &uitleg));
    MOCK_METHOD1(SetLangzaamRijdenUitleg, void(const std::string &uitleg));

};

#endif // IREGELAARINSTELLINGENDIALOOGMOCK_H
