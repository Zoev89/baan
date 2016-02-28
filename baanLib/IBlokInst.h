#ifndef IBLOKINST_H
#define IBLOKINST_H
#include <string>

class IBlokInst
{
public:
    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetBlok(int blokNummer) = 0;
    virtual int   GetBlok() = 0;
    virtual void  SetVolgendBlok(const std::string& blok) = 0;
    virtual std::string GetVolgendBlok() = 0;
    virtual void  SetMaxSnelheid(int maxSnelheid) = 0;
    virtual int   GetMaxSnelheid() = 0;
    virtual void  SetLengte(int lengte) = 0;
    virtual int   GetLengte() = 0;
    virtual void  SetBovenLeiding(bool bovenleiding) = 0;
    virtual bool  GetBovenLeiding() = 0;
    virtual void  SetSein(int seinIndex) = 0;
    // seinIndex waardes
    // 0 = GEEN_SEIN:
    // 1 = ROOD_GROEN_SEIN:
    // 2 = ROOD_GROEN_GEEL_SEIN:
    virtual int   GetSein() = 0;
    virtual void  SetVoorKeurRichting(int voorKeurRichtingIndex) = 0;
    virtual int   GetVoorKeurRichting() = 0;
};

#endif // IBLOKINST_H
