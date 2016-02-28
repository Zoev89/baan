#ifndef IREGELAARINSTELLINGENDIALOG_H
#define IREGELAARINSTELLINGENDIALOG_H
#include <string>

class IRegelaarInstellingenDialoog
{
public:
    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetLaatsteWagonCheck(bool laatsteWagon) = 0;
    virtual bool  GetLaatsteWagonCheck() = 0;
    virtual void  SetEloc(bool eloc) = 0;
    virtual bool  GetEloc() = 0;
    virtual void  SetMinSnelheid(int snelheid) = 0;
    virtual int   GetMinSnelheid() = 0;
    virtual void  SetMaxSnelheid(int snelheid) = 0;
    virtual int   GetMaxSnelheid() = 0;
    virtual void  SetTopSnelheid(int snelheid) = 0;
    virtual int   GetTopSnelheid() = 0;
    virtual void  SetTotaalAfstand(int afstand) = 0; // in m
    virtual int   GetTotaalAfstand() = 0;
    virtual void  SetTotaalTijd(const std::string& tijd) = 0; // hh:mm
    virtual std::string GetTotaalTijd() = 0;
    virtual void  SetLengte(int lengte) = 0; // in cm
    virtual int   GetLengte() = 0;
    virtual void  SetAlphaRijden(float alpha) = 0;
    virtual float GetAlphaRijden() = 0;
    virtual void  SetClipRijden(int clip) = 0;
    virtual int   GetClipRijden() = 0;
    virtual void  SetAlphaStoppen(float alpha) = 0;
    virtual float GetAlphaStoppen() = 0;
    virtual void  SetClipStoppen(int clip) = 0;
    virtual int   GetClipStoppen() = 0;
    virtual void  SetStand1(int snelheid) = 0;
    virtual int   GetStand1() = 0;
    virtual void  SetAfstand1(int afstand) = 0; // in cm
    virtual int   GetAfstand1() = 0;
    virtual void  SetStand2(int snelheid) = 0;
    virtual int   GetStand2() = 0;
    virtual void  SetAfstand2(int afstand) = 0; // in cm
    virtual int   GetAfstand2() = 0;
    virtual void  SetErrors(const std::string& errorString) = 0;
    virtual std::string GetErrors() = 0;
    virtual void  SetKLpf(float k) = 0;
    virtual float GetKLpf() = 0;
    virtual void  SetPlusMinus(float plusMinus) = 0;
    virtual float GetPlusMinus() = 0;
    virtual void  SetHelling(float helling) = 0;
    virtual float GetHelling() = 0;
    virtual void  SetDodeTijd(float dodeTijd) = 0;
    virtual float GetDodeTijd() = 0;
    virtual void  SetProgrammaNaam(const std::string& naam) = 0;
    virtual std::string GetProgrammaNaam() = 0;
    virtual void  SetHerlaadProgramma(bool herlaad) = 0;
    virtual bool  GetHerlaadProgramma() = 0;
    virtual void  SetLangzaam(int snelheid) = 0;
    virtual int   GetLangzaam() = 0;
    virtual void  SetRijden(int snelheid) = 0;
    virtual int   GetRijden() = 0;
    virtual void  SetLocType(const std::string& locType) = 0;
    virtual std::string GetLocType() = 0;
    virtual void  SetLastStand1(int stand) = 0;
    virtual int   GetLastStand1() = 0;
    virtual void  SetLastGain1(float gain) = 0;
    virtual float GetLastGain1() = 0;
    virtual void  SetLastStand2(int stand) = 0;
    virtual int   GetLastStand2() = 0;
    virtual void  SetLastGain2(float gain) = 0;
    virtual float GetLastGain2() = 0;
    virtual void  SetLastRegelKeuze(int keuzeIndex) = 0;
    virtual int   GetLastRegelKeuze() = 0;
};

#endif // IREGELAARINSTELLINGDIALOG_H
