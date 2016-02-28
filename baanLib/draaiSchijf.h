// BaanWT.h: interface for the BaanWorkerThreadInfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "IIoBits.h"
#include "IBlok.h"
#include "IWissels.h"
#include "IMainWindowDrawing.h"
#include "IBaanMessage.h"
#include "IWisselDialoog.h"

class DraaiSchijf: public IOBits_t
{
public:
    DraaiSchijf(IMessage& msg, IBlok& blok, IWissels& wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, IWisselDialoog& standaardWisselDialoog, BaanInfo_t *baanInfo, int kopBlok);
    ~DraaiSchijf();
    // IWissel
    virtual int Init(char *Input, FILE *file) override;
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
    void UpdateRec ();
    void Bedien ();
    IMessage& mMessage;
    IBlok& mBlok;
    IWissels& mWissels;
    IBaanMessage& mBaanMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    IWisselDialoog& mStandaardWisselDialoog;
    BaanInfo_t* mBaanInfo;

    // middelpunt is het middelpunt van de rect
    std::vector<DraaiSchijfAansluiting> aansluiting;

    BlokPointer_t *pBlok1;	/* altijd een baan blok */

};

