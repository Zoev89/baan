#ifndef IKOPRICHTINGDIALOOG_H
#define IKOPRICHTINGDIALOOG_H
#include <string>

class IKopRichtingDialoog
{
public:
    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetKopBlok(int blok) = 0;
    virtual int  GetKopBlok() = 0;
    virtual void  SetLengte(int lengte) = 0;
    virtual int  GetLengte() = 0;
    virtual void  SetKopBlokMee(bool check) = 0;
    virtual bool  GetKopBlokMee() = 0;
    virtual void  SetVooruitTerug(bool sliderVT) = 0;
    virtual bool  GetVoorUitTerug() = 0;
    virtual void SetErrorText(const std::string& errorText) = 0;
    virtual std::string GetErrorText() = 0;
};

#endif // IKOPRICHTINGDIALOOG_H
