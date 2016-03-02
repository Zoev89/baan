#if !defined(_LAMP_H_)
#define _LAMP_H_
#include "IIoBits.h"
#include "IWissel.h"
#include "IBlok.h"
#include "IWissels.h"
#include "IBaanMessage.h"
#include "IMainWindowDrawing.h"
#include "IWisselDialoog.h"

class Lamp: public IOBits_t
{
public:
    Lamp(IMessage& msg, IBlok& blok, IWissels& wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, IWisselDialoog& lampDialoog, BaanInfo_t *baanInfo);
    // IWissel
    virtual int Init(const char *Input, std::function<std::string()> newInput) override;
    virtual void InitRoutering () override;
    virtual void ZoekOngeinitializeerdeEindBlokken() override;
    virtual void Display () override;
    virtual int Aanvraag (int stand) override;
    virtual void String(char *string) override;
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
    int WisselNummer();
    IMessage& mMessage;
    IBlok& mBlok;
    IWissels& mWissels;
    IBaanMessage& mBaanMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    IWisselDialoog& mLampDialoog;
    BaanInfo_t *mBaanInfo;

    int UitTijd;
    int aan;
    // de variablen hwAan hwTot hwType worden niet door
    // het programma gebruikt maar ik moet ze wel opslaan
    // om later een blok file te kunnen generen
    int hwAan;
    int hwTot;
    int hwType;
};

#endif // !defined(_LAMP_H_)
