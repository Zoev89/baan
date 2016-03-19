// BaanWT.h: interface for the BaanWorkerThreadInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ONTKOPPELAAR_H_)
#define _ONTKOPPELAAR_H_

#include "IIoBits.h"
#include "IBlok.h"
#include "IWissels.h"
#include "IWisselDialoog.h"

class Ontkoppelaar: public IOBits_t
{
public:
    Ontkoppelaar(IMessage& msg, IBlok& blok, IWissels & wissels, IWisselDialoog& ontKoppelaarDialoog, BaanInfo_t *baanInfo);

    // IWissel
    virtual int Init(const char *Input, std::function<std::string()> newInput) override;
    virtual void InitRoutering () override;
    virtual void ZoekOngeinitializeerdeEindBlokken() override;
    virtual void Display () override;
    virtual int Aanvraag (int stand) override;
    virtual std::string String() override;
    virtual void NieuwXY (int selectionX, int selectionY, int deltaX, int deltaY) override;
    virtual void InitDialoog () override;
    virtual void DialoogOk () override;
    virtual void TestIOTimer () override;
    virtual float GetAdres() override;
    virtual int CheckWissel() override;
    virtual void VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok) override;
    virtual BlokPointer_t * GetWisselBlok() override;
    virtual int GetRichting() override;
    virtual int WisselLengte(int from, int to) override;


private:
    IMessage& mMessage;
    IBlok& mBlok;
    IWissels& mWissels;
    IWisselDialoog& mOntKoppelaarDialoog;
    BaanInfo_t *mBaanInfo;

    int InactiefX1;
    int InactiefY1;
    int InactiefX2;
    int InactiefY2;
    int ActiefX1;
    int ActiefY1;
    int ActiefX2;
    int ActiefY2;
};

#endif // !defined(_ONTKOPPELAAR_H_)
