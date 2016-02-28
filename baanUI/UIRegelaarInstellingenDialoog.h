#ifndef UIREGELAARINSTELLINGENDIALOOG_H
#define UIREGELAARINSTELLINGENDIALOOG_H
#include "IRegelaarInstellingenDialoog.h"

class UIRegelaarInstellingenDialoog : public IRegelaarInstellingenDialoog
{
public:
    UIRegelaarInstellingenDialoog();

    virtual bool RunDialogOk(); // returns true when ok is pressed

    virtual void  SetLaatsteWagonCheck(bool laatsteWagon);
    virtual bool  GetLaatsteWagonCheck();
    virtual void  SetEloc(bool eloc);
    virtual bool  GetEloc();
    virtual void  SetMinSnelheid(int snelheid);
    virtual int   GetMinSnelheid();
    virtual void  SetMaxSnelheid(int snelheid);
    virtual int   GetMaxSnelheid();
    virtual void  SetTopSnelheid(int snelheid);
    virtual int   GetTopSnelheid();
    virtual void  SetTotaalAfstand(int afstand); // in m
    virtual int   GetTotaalAfstand();
    virtual void  SetTotaalTijd(const std::string& tijd); // hh:mm
    virtual std::string GetTotaalTijd();
    virtual void  SetLengte(int lengte); // in cm
    virtual int   GetLengte();
    virtual void  SetAlphaRijden(float alpha);
    virtual float GetAlphaRijden();
    virtual void  SetClipRijden(int clip);
    virtual int   GetClipRijden();
    virtual void  SetAlphaStoppen(float alpha);
    virtual float GetAlphaStoppen();
    virtual void  SetClipStoppen(int clip);
    virtual int   GetClipStoppen();
    virtual void  SetStand1(int snelheid);
    virtual int   GetStand1();
    virtual void  SetAfstand1(int afstand); // in cm
    virtual int   GetAfstand1();
    virtual void  SetStand2(int snelheid);
    virtual int   GetStand2();
    virtual void  SetAfstand2(int afstand); // in cm
    virtual int   GetAfstand2();
    virtual void  SetErrors(const std::string& errorString);
    virtual std::string GetErrors();
    virtual void  SetKLpf(float k);
    virtual float GetKLpf();
    virtual void  SetPlusMinus(float plusMinus);
    virtual float GetPlusMinus();
    virtual void  SetHelling(float helling);
    virtual float GetHelling();
    virtual void  SetDodeTijd(float dodeTijd);
    virtual float GetDodeTijd();
    virtual void  SetProgrammaNaam(const std::string& naam);
    virtual std::string GetProgrammaNaam();
    virtual void  SetHerlaadProgramma(bool herlaad);
    virtual bool  GetHerlaadProgramma();
    virtual void  SetLangzaam(int snelheid);
    virtual int   GetLangzaam();
    virtual void  SetRijden(int snelheid);
    virtual int   GetRijden();
    virtual void  SetLocType(const std::string& locType);
    virtual std::string GetLocType();
    virtual void  SetLastStand1(int stand);
    virtual int   GetLastStand1();
    virtual void  SetLastGain1(float gain);
    virtual float GetLastGain1();
    virtual void  SetLastStand2(int stand);
    virtual int   GetLastStand2();
    virtual void  SetLastGain2(float gain);
    virtual float GetLastGain2();
    virtual void  SetLastRegelKeuze(int keuzeIndex);
    virtual int   GetLastRegelKeuze();

};


#endif // UIREGELAARINSTELLINGENDIALOOG_H
