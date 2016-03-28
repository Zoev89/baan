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
#include "Dispatcher.h"
#include "IThreadSleep.h"
#include <boost/optional.hpp>

class DraaiSchijf: public IOBits_t
{
public:
    DraaiSchijf(IMessage& msg, IBlok& blok, IWissels& wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, IWisselDialoog& standaardWisselDialoog, BaanInfo_t *baanInfo, IThreadSleep & threadSleep, int kopBlok);
    virtual ~DraaiSchijf();
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
    void WachtOp(int status);
    bool GaNaarPositie(int positie) ;
    void Bedien (int adres, int data, bool returnGewenst=false);
    int WisselNummer();
    int checkAansluiting(DraaiSchijfAansluiting &aansturing);
    void DraaiSchijfBlokAangesproken();

    IMessage& mMessage;
    IBlok& mBlok;
    IWissels& mWissels;
    IBaanMessage& mBaanMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    IWisselDialoog& mDraaiWisselDialoog;
    BaanInfo_t* mBaanInfo;
    IThreadSleep &mThreadSleep;
    bool mStopWorker;
    Dispatcher mWorker;

    int Coord1X;
    int Coord1Y;
    int Radius;
    int OffsetAngle;
    int NieuweStand; // geeft de positie aan waar de lock binnenkomt dat is niet de draaischijf stand
    bool StartDrag;  // click drag dan kan je draaien en bevestig met click
    bool AndereKant; // de andere kant word gebruikt
    bool mTurning;
    bool mIsStarted;
    // middelpunt is het middelpunt van de rect
    std::array<boost::optional<DraaiSchijfAansluiting>, 48> m_aansluitingen;

    BlokPointer_t *pBlok1;	/* altijd een baan blok */

};

