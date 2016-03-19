// BaanWT.h: interface for the BaanWorkerThreadInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STANDAARDWISSEL_H_)
#define _STANDAARDWISSEL_H_

#include "IIoBits.h"
#include "IBlok.h"
#include "IWissels.h"
#include "IMainWindowDrawing.h"
#include "IBaanMessage.h"
#include "IWisselDialoog.h"

class StandaardWissel: public IOBits_t
{
public:
    StandaardWissel(IMessage& msg, IBlok& blok, IWissels& wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, IWisselDialoog& standaardWisselDialoog, BaanInfo_t *baanInfo, int kopBlok);
    ~StandaardWissel();
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
    void UpdateRec ();
    int WisselNummer();
    void Bedien ();
    IMessage& mMessage;
    IBlok& mBlok;
    IWissels& mWissels;
    IBaanMessage& mBaanMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    IWisselDialoog& mStandaardWisselDialoog;
    BaanInfo_t* mBaanInfo;

    int Coord1X;
    int Coord1Y;
    int Coord2X;
    int Coord2Y;
    int Coord3X;
    int Coord3Y;
    BlokPointer_t *pBlok1;	/* altijd een baan blok */
    int Richting;
    int MaxSnelheid12;
    int MaxSnelheid13;
    int Lengte12;
    int Lengte13;

};

#endif // !defined(_BAANWT_H_)
