#include "baanView.h"
#include "baanDoc.h"
#include "baanTypes.h"
#include "baanWT.h"
#include <stdio.h>
#include <string.h>
#include "wissel.h"
#include "blok.h"
#include "td.h"
#include <boost/format.hpp>

BaanView::BaanView (IMessage &msg, IBaanDoc& baanDoc, IBlok& blok, IBlokInst& blokInst
                    , IWissels &wissels, IMainWindowDrawing &mainWindowDrawing, ITd &td, BaanInfo_t *baanInfo):
    mMessage(msg),
    mBaanDoc(baanDoc),
    mBlok(blok),
    mBlokInst(blokInst),
    mWissels(wissels),
    mMainWindowDrawing(mainWindowDrawing),
    mTd(td),
    mBaanInfo(baanInfo)
{
    tdview = mTd.tdCreate("view");
    tdDraw = mTd.tdCreate("draw");

    tdOnBlokEvent = mTd.tdValueString ("OnBlokEvent");
    tdOnTimerVerlopen = mTd.tdValueString ("OnTimerVerlopen");
    tdOnInternalTimerVerlopen = mTd.tdValueString ("OnInternalTimerVerlopen");
}


void
BaanView::baanViewBlokXY (int blok, int x, int y)
{
    mBlok.BlokDisplay (blok, -1, BLOK_REMOVE);
    mBaanInfo->Blok[blok].XCoord = x;
    mBaanInfo->Blok[blok].YCoord = y;
    mBlok.BlokDisplay (blok, -1, BLOK_CLEAR);
}

// handle mouse events
//
// return handled or not
bool
BaanView::HandleMouseEvents (MouseEventType type, MouseButtonType button,int x, int y)
{
    int i;

    static int vorigEvent;
    static int vorigKey;
    static int vorigX;
    static int vorigY;
    bool handled = false;

    if (ButtonClick == type)
    {
        if (mBaanInfo->editMode)
        {
            // kijk eerst eens of we een blok selecteren
            if (mBaanInfo->selectedBlok)
            {
                int reset = mBaanInfo->selectedBlok;
                mBaanInfo->selectedBlok = 0;
                mBlok.BlokDisplay (reset, -1, BLOK_CLEAR);
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                auto reset = mBaanInfo->selectedWissel;
                mBaanInfo->selectedWissel = NULL;
                mBaanInfo->selectedWisselPoint = -1;
                mBaanInfo->selectedWisselX = -1;
                mBaanInfo->selectedWisselY = -1;
                reset->Display();
            }

        }
        if (LeftButton == button)
        {
            mBaanDoc.baanDocLeftMouseButtonDown (x, y);
        }

        if (RightButton == button)
        {
            mBaanDoc.baanDocRightMouseButtonDown (x , y);
            if ((vorigEvent == ButtonClick) && (vorigKey == button) &&
                    (vorigX == x) && (vorigY == y))
            {
                if (mBaanInfo->selectedBlok)
                {
                    char string[10];

                    mBaanDoc.baanDocSetWisselsDefault ();
                    mBlokInst.SetBlok(mBaanInfo->selectedBlok);
                    mBlok.BlokNaam (string,
                                    mBaanInfo->BlokPointer[mBaanInfo->selectedBlok].pVolgendBlok);
                    mBlokInst.SetVolgendBlok(string);
                    mBlokInst.SetLengte(mBaanInfo->
                                        BlokPointer[mBaanInfo->selectedBlok].Lengte);
                    if (mBaanInfo->Blok[mBaanInfo->selectedBlok].MaxSnelheid != 1000)
                    {
                        mBlokInst.SetMaxSnelheid(mBaanInfo->Blok
                                                 [mBaanInfo->selectedBlok].MaxSnelheid);
                    }
                    else
                    {
                        mBlokInst.SetMaxSnelheid(-1);
                    }
                    mBlokInst.SetBovenLeiding (mBaanInfo->
                                               Blok[mBaanInfo->selectedBlok].Bovenleiding
                                               == 1);


                    switch (mBaanInfo->Blok[mBaanInfo->selectedBlok].blokSein)
                    {
                    case GEEN_SEIN:
                        mBlokInst.SetSein(0);
                        break;
                    case ROOD_GROEN_SEIN:
                        mBlokInst.SetSein(1);
                        break;
                    case ROOD_GROEN_GEEL_SEIN:
                        mBlokInst.SetSein(2);
                        break;
                    }
                    
                    mBlokInst.SetVoorKeurRichting(
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].richtingVoorkeur - richtingVooruit);

                    if (mBlokInst.RunDialogOk())
                    {
                        if ((mBlokInst.GetBlok() != mBaanInfo->selectedBlok)
                                || (strcmp (mBlokInst.GetVolgendBlok().c_str(), string)
                                    != 0))
                        {
                            int nieuwBlok = mBlokInst.GetBlok ();
                            BlokPointer_t *volgendBlok;
                            BlokPointer_t *vorigBlok;
                            volgendBlok =
                                    mBaanInfo->BlokPointer[mBaanInfo->selectedBlok].pVolgendBlok;
                            if ((nieuwBlok > 0) && (nieuwBlok < MAX_NOBLOKS))
                            {
                                if (nieuwBlok != mBaanInfo->selectedBlok)
                                {
                                    if (mBaanInfo->
                                            BlokPointer[nieuwBlok].BlokIONummer !=
                                            -1)
                                    {
                                        mMessage.message
                                                (str(boost::format("Het nieuwe blok %d is al bezet wijziging niet overgenomen") %
                                                     nieuwBlok));
                                        nieuwBlok = mBaanInfo->selectedBlok;
                                    }
                                }
                                // nieuwBlok wijst naar een correct blok
                                if (strcmp
                                        (mBlokInst.GetVolgendBlok().c_str(),
                                         string) != 0)
                                {
                                    char blokType[2];
                                    float adres;

                                    // volgend blok gewijzigd
                                    if (sscanf
                                            (mBlokInst.GetVolgendBlok().c_str(), "%1s%f",
                                             blokType, &adres) == 2)
                                    {
                                        volgendBlok =
                                                mWissels.wisselKrijgPointer (blokType[0], adres);
                                        if (volgendBlok == NULL)
                                        {
                                            mMessage.message
                                                    (str(boost::format("Het volgend blok bestaat niet %s") %
                                                         mBlokInst.GetVolgendBlok()));
                                            // wijziging ongedaan gemaakt
                                            volgendBlok =
                                                    mBaanInfo->BlokPointer
                                                    [mBaanInfo->selectedBlok].pVolgendBlok;
                                        }
                                    }
                                    else
                                    {
                                        mMessage.message
                                                (str(boost::format("Het volgend blok voldoet niet aan de B/W/w %s") %
                                                     mBlokInst.GetVolgendBlok()));
                                        volgendBlok =
                                                mBaanInfo->BlokPointer
                                                [mBaanInfo->selectedBlok].pVolgendBlok;
                                    }
                                }

                                // Nu zijn volgendBlok en nieuwBlok altijd correcte dingen nu kunnen we gaan wijzigen
                                vorigBlok =
                                        mBaanInfo->BlokPointer[mBaanInfo->selectedBlok].pVorigBlok;
                                mBlok.BlokPrint (vorigBlok);
                                mBlok.BlokDelete (&mBaanInfo->BlokPointer
                                                  [mBaanInfo->selectedBlok]);
                                mBaanInfo->BlokPointer[nieuwBlok].pVolgendBlok =
                                        volgendBlok;
                                mBaanInfo->BlokPointer[nieuwBlok].pVorigBlok =
                                        vorigBlok;
                                mBlok.BlokPrint (volgendBlok);
                                mBlok.BlokPrint (vorigBlok);

                                mBlok.BlokInsert (&mBaanInfo->BlokPointer[nieuwBlok]);
                                mBaanInfo->BlokPointer[nieuwBlok].BlokIONummer =
                                        nieuwBlok;
                                mBaanInfo->Blok[nieuwBlok].XCoord =
                                        mBaanInfo->Blok[mBaanInfo->selectedBlok].XCoord;
                                mBaanInfo->Blok[nieuwBlok].YCoord =
                                        mBaanInfo->Blok[mBaanInfo->selectedBlok].YCoord;
                                if (mBaanInfo->selectedBlok != nieuwBlok)
                                {
                                    // update de wissels hun pBlok en kruising zijn schaduw blok
                                    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
                                    {
                                        mWissels.WisselVeranderBlok (i,
                                                                     &mBaanInfo->BlokPointer[mBaanInfo->selectedBlok],
                                                                     &mBaanInfo->BlokPointer[nieuwBlok]);
                                    }
                                }

                                mBaanInfo->selectedBlok = nieuwBlok;
                            }
                            else
                            {
                                mMessage.message
                                        (str(boost::format("Blok nummer %d niet binnen de range [1..%d]") %
                                             nieuwBlok % (MAX_NOBLOKS - 1)));
                                nieuwBlok = mBaanInfo->selectedBlok;
                            }
                        }
                        mBaanInfo->Blok[mBaanInfo->selectedBlok].blokSein =
                                (blokSein_t) mBlokInst.GetSein ();
                        mBaanInfo->BlokPointer[mBaanInfo->selectedBlok].Lengte =
                                mBlokInst.GetLengte ();
                        if (mBlokInst.GetMaxSnelheid() > 0)
                        {
                            mBaanInfo->Blok[mBaanInfo->selectedBlok].MaxSnelheid =
                                    mBlokInst.GetMaxSnelheid();
                        }
                        else
                        {
                            mBaanInfo->Blok[mBaanInfo->selectedBlok].MaxSnelheid = 1000;
                        }
                        mBaanInfo->Blok[mBaanInfo->selectedBlok].Bovenleiding =
                                mBlokInst.GetBovenLeiding ();
                        mBaanInfo->Blok[mBaanInfo->selectedBlok].richtingVoorkeur =
                                (BlokRichtingVoorkeur) (mBlokInst.GetVoorKeurRichting() + richtingVooruit);
                        mMainWindowDrawing.RedrawAll();
                    }
                }
                if (mBaanInfo->selectedWissel != NULL)
                {
                    mBaanDoc.baanDocSetWisselsDefault ();
                    {
                        if (mWissels.InitDialoog(mWissels.WisselIndex(mBaanInfo->selectedWissel)))
                        {
                            mWissels.DialoogOk (mWissels.WisselIndex(mBaanInfo->selectedWissel));
                        }
                    }
                }
            }

        }
        handled = true;
    }
    else if (ButtonDrag == type)
    {
        if (button == RightButton)
        {
            if (mBaanInfo->selectedBlok)
            {
                baanViewBlokXY (mBaanInfo->selectedBlok, x - mBaanInfo->selectedOffsetX,
                                y - mBaanInfo->selectedOffsetY);
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                // wisselNieuwXY werkt met verschil updates
                mBaanInfo->selectedWissel->NieuwXY (mBaanInfo->selectedOffsetX,
                                  mBaanInfo->selectedOffsetY, x - mBaanInfo->selectedWisselX,
                                  y - mBaanInfo->selectedWisselY);
                // onthoud het huidige punt
                mBaanInfo->selectedWisselX = x;
                mBaanInfo->selectedWisselY = y;
            }
            handled = true;
        }
        else
        {
            // Controlle of een spoel zich aangesproken voelt
            for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
            {
                if (mBaanInfo->IOBits[i].get()->Type)
                {
                    if ((x >= mBaanInfo->IOBits[i].get()->rec.x ()) &&
                            (x <= mBaanInfo->IOBits[i].get()->rec.r ()) &&
                            (y >= mBaanInfo->IOBits[i].get()->rec.y ()) &&
                            (y <= mBaanInfo->IOBits[i].get()->rec.b ()))
                    {
                        IOAanvraag_t IOAanvraag;

                        // Het gegeven punt ligt in deze wissel dus roep
                        // de workthread aan met een wissel request
                        IOAanvraag.stand = IOAANVRAAG_TOGGLE | x<<16 | y;
                        IOAanvraag.IONummer = i;
                        mWissels.Aanvraag (&IOAanvraag);

                        i = mBaanInfo->AantalSpoelen;
                    }
                }
            }


        }
    }

    vorigKey = button;
    vorigEvent = type;
    vorigX = x;
    vorigY = y;

    return handled; // if you used the event
}


bool BaanView::HandleKeyBoardEvent(KeyBoardEventType key)
{
    {
        switch (key)
        {
        case RightKey:
            if (mBaanInfo->selectedBlok)
            {
                baanViewBlokXY (mBaanInfo->selectedBlok,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].XCoord + 1,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].YCoord);
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                mBaanInfo->selectedWissel->NieuwXY ( mBaanInfo->selectedOffsetX,
                                  mBaanInfo->selectedOffsetY, 1, 0);
            }
            break;
        case LeftKey:
            if (mBaanInfo->selectedBlok)
            {
                baanViewBlokXY (mBaanInfo->selectedBlok,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].XCoord - 1,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].YCoord);
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                mBaanInfo->selectedWissel->NieuwXY (mBaanInfo->selectedOffsetX,
                                  mBaanInfo->selectedOffsetY, -1, 0);
            }
            break;
        case UpKey:
            if (mBaanInfo->selectedBlok)
            {
                baanViewBlokXY (mBaanInfo->selectedBlok,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].XCoord,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].YCoord - 1);
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                mBaanInfo->selectedWissel->NieuwXY ( mBaanInfo->selectedOffsetX,
                                  mBaanInfo->selectedOffsetY, 0, -1);
            }
            break;
        case DownKey:
            if (mBaanInfo->selectedBlok)
            {
                baanViewBlokXY (mBaanInfo->selectedBlok,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].XCoord,
                                mBaanInfo->Blok[mBaanInfo->selectedBlok].YCoord + 1);
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                mBaanInfo->selectedWissel->NieuwXY (mBaanInfo->selectedOffsetX,
                                  mBaanInfo->selectedOffsetY, 0, 1);
            }
            break;
        case DeleteKey:
            if (mBaanInfo->selectedBlok)
            {
                mBaanDoc.baanDocSetWisselsDefault ();
                mBlok.BlokDelete (&mBaanInfo->BlokPointer[mBaanInfo->selectedBlok]);
                mBaanInfo->AantalBlokken -= 1;
                mBaanInfo->selectedBlok = 0;
                mMainWindowDrawing.RedrawAll();
            }
            if (mBaanInfo->selectedWissel != NULL)
            {
                mBaanDoc.baanDocSetWisselsDefault ();
                mWissels.WisselDelete (mWissels.WisselIndex(mBaanInfo->selectedWissel));
                mBaanInfo->selectedWissel = NULL;
                mBaanInfo->selectedWisselPoint = -1;
                mMainWindowDrawing.RedrawAll();
            }
        }
    }
    return true;
}


void
BaanView::baanViewVierkantDisplay (int xy, int lengteHoogte, IMainWindowDrawing::BaanColor color)
{
    mMainWindowDrawing.VierkantDisplay(xy & 0xffff, xy >> 16, lengteHoogte & 0xffff, lengteHoogte >> 16, color);
}


void
BaanView::Draw ()
{
    int i;

    mTd.tdStart (tdDraw);

    if (mBaanInfo->AantalBlokken == 0)
        return;

    IMainWindowDrawing::BaanColor color;
    if (mBaanInfo->Stop)
        color = IMainWindowDrawing::Red;
    else
        color = IMainWindowDrawing::Green;

    baanViewVierkantDisplay (mBaanInfo->EindBlok.XCoord +
                             (mBaanInfo->EindBlok.YCoord << 16),
                             RESET_SIZE + (RESET_SIZE << 16), color);


    for (i = 1; i < MAX_NOBLOKS; i++)
    {
        int regelaar, actie = BLOK_CLEAR;
        regelaar = mBaanInfo->Blok[i].RegelaarNummer;
        if (regelaar != -1)
        {
            if ((mBaanInfo->Blok[i].State == BLOK_ACHTERUITCHECK) ||
                    (mBaanInfo->Blok[i].State == BLOK_VOORUITCHECK))
            {
                actie = BLOK_KOPBLOK;
            }
            else
            {
                actie = BLOK_BELEG;
            }
        }
        else
        {
            if ((mBaanInfo->Blok[i].blokBelegt) &&
                    (mBaanInfo->Blok[i].blokBelegtRegelaar != -1))
            {
                regelaar = mBaanInfo->Blok[i].blokBelegtRegelaar;
                actie = BLOK_RESERVED;
            }
        }
        if (mBaanInfo->BlokPointer[i].BlokIONummer != -1)
        {
            mBlok.BlokDisplay (i, regelaar, actie);
        }
    }
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        mWissels.Display (i, true);
    }

    mTd.tdStop (tdDraw);
}


void
BaanView::Update (int type, int value1, int value2, int value3)
{
    mTd.tdStart(tdview);

    switch (type)
    {
    case WM_WISSEL_DISPLAY:
        mWissels.Display(value1, true);
        break;
    case WM_BLOK_DISPLAY:
        mBlok.BlokDisplay (value1, value2, value3);
        break;
    case WM_VIERKANT_DISPLAY:
        baanViewVierkantDisplay (value1, value2, static_cast<IMainWindowDrawing::BaanColor>(value3));
        break;
    case WM_BLOK_EVENT:
        int regelaar, event;
        mTd.tdValue (tdOnBlokEvent, 1);
        regelaar = value1;
        event = value2;
        // als event == -1 dan wordt het runnende programma gestopt
        if (mBaanInfo->RegelArray[regelaar].programRunning)
        {
            // programma wordt weer actief
            mBaanDoc.baanDocExecuteProgram (regelaar, BLOK, event);
        }
        else
        {
            if (event == -1)
            {
                mBaanDoc.baanDocStopProgram (regelaar);
            }
        }
        break;
    case WM_INTERNALTIMER_VERLOPEN:
        regelaar = value1;
        event = value2;
        mTd.tdValue (tdOnInternalTimerVerlopen, event);
        mBaanDoc.baanDocExecuteProgram (regelaar, INTERNAL, event);
        break;

    case WM_TIMER_VERLOPEN:
        regelaar = value1;
        event = value2;
        mTd.tdValue (tdOnTimerVerlopen, event);
        mBaanDoc.baanDocExecuteProgram (regelaar, TIMER, event);
        break;

    }

    mTd.tdStop (tdview);
}
