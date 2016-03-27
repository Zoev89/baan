// Baan: A railway model controlling program
// Copyright (C) 1998 - 2001 Eric Kathmann
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA  02111-1307, USA.

#include "baan.h"
#include "baanTypes.h"
#include "baanWT.h"
#include "wissel.h"
#include "draaiSchijf.h"
#include "baanMessage.h"
#include "leesdata.h"
#include "blok.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/format.hpp>
#include <cmath>

#define Homing 60
#define EnableMiddenDetectie 61
#define GetStatus 63
#define Turning 1
#define NeedsHoming 0x4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DraaiSchijf::DraaiSchijf(IMessage &msg, IBlok& blok, IWissels &wissels, IBaanMessage &baanMessage, IMainWindowDrawing &mainWindowDrawing, IWisselDialoog &standaardWisselDialoog, BaanInfo_t *baanInfo, IThreadSleep & threadSleep, int kopBlok)
    : mMessage(msg)
    , mBlok(blok)
    , mWissels(wissels)
    , mBaanMessage(baanMessage)
    , mMainWindowDrawing(mainWindowDrawing)
    , mDraaiWisselDialoog(standaardWisselDialoog)
    , mBaanInfo(baanInfo)
    , mThreadSleep(threadSleep)
    , mStopWorker(false)
    , Coord1X(30)
    , Coord1Y(30)
    , Radius(20)
    , OffsetAngle(0)
    , NieuweStand(100)
    , StartDrag(false)
    , AndereKant(false)
{
    routeKnoopPunt.resize(3);
    pBlok1 = &mBaanInfo->BlokPointer[kopBlok];

}

DraaiSchijf::~DraaiSchijf()
{
    mStopWorker = true;
}

BlokPointer_t * DraaiSchijf::GetWisselBlok()
{
    return pBlok1;
}

int DraaiSchijf::GetRichting()
{
    return 0;
}

int DraaiSchijf::WisselLengte(int , int )
{
    return 0;
}

void DraaiSchijf::VeranderBlok (BlokPointer_t * , BlokPointer_t * )
{
}


int DraaiSchijf::CheckWissel()
{
//    if (CheckWisselBlok (pBlok1, Richting, &mBaanInfo->EindBlokPointer))
//    {
//        mMessage.message(str(boost::format
//        ("DraaiSchijf %d.%02d aansluiting 2 niet goed aangesloten!") %
//         hardwareAdres% hardwareBit));
//        return 1;
//    }
//    if (CheckWisselBlok(&StopBlokPointer[0], Richting, &mBaanInfo->EindBlokPointer))
//    {
//        mMessage.message(str(boost::format
//        ("DraaiSchijf %d.%02d aansluiting 3 niet goed aangesloten!") %
//          hardwareAdres% hardwareBit));
//        return 1;
//    }
    return 0;
}

// TODO remove this when mBaanMessage.Post supports a std::function
int DraaiSchijf::WisselNummer()
{
    for (size_t i=0;i<mBaanInfo->IOBits.size();i++)
    {
        if (this == mBaanInfo->IOBits[i].get())
        {
            return static_cast<int>(i);
        }
    }
    return 0;
}

void DraaiSchijf::TestIOTimer ()
{

}

float DraaiSchijf::GetAdres()
{
    return mDraaiWisselDialoog.GetAdres();
}

void DraaiSchijf::UpdateRec ()
{
    // implementeer iets
    rec.x (Coord1X-Radius-2);
    rec.y (Coord1Y-Radius-2);
    rec.w (2*Radius+4);
    rec.h (2*Radius+6);
}

int DraaiSchijf::Init (const char *Input, std::function<std::string()> extraInput)
{
    float floatAdres;
    int aantal;

    /* Lees alle velden in */
    if (sscanf (Input, "%d%f%d%d%d%d%d",
                &Type,
                &floatAdres,
                &Coord1X,
                &Coord1Y,
                &Radius,
                &OffsetAngle,
                &aantal
                ) != 7)
        return WISSEL_ERR_NIET_ALLES_AANWEZIG;
    hardwareAdres = (int)floatAdres;
    hardwareBit = 0;
    NieuweStand = Stand = 100;
    hardwareReturnWaarde = 0;
    for (int i=0;i<aantal;++i)
    {
        auto string = extraInput();
        DraaiSchijfAansluiting aansturing;
        int gnd,vt;
        if (sscanf (string.c_str(), "%d%d%d%d",
                    &aansturing.aansluitingNummer,
                    &gnd,
                    &aansturing.blok,
                    &vt) != 4)
            return WISSEL_ERR_NIET_ALLES_AANWEZIG;
        aansturing.gndFirst = (gnd == 0) ? false: true;
        aansturing.richtingVooruit = (vt == 0) ? false: true;
        auto err = checkAansluiting(aansturing);
        if (err)
            return err;
        if ((aansturing.aansluitingNummer < 0) || (aansturing.aansluitingNummer >= 48))
        {
            mMessage.message(str(boost::format
                                 ("Foutief aansluiting nummer at %d voor draaischijf %d string %s") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                  string.c_str()));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }
        // check  of the baanblok aan die kant vrij is
        if (mBlok.BlokIsBlokNummer(aansturing.blok)==0)
        {
            mMessage.message(str(boost::format
                                 ("Foutief blok nummer at %d voor draaischijf %d blok nummer %d") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                  aansturing.blok));
            return WISSEL_ERR_INVALID_ADRES;
        }
        if (mBaanInfo->BlokPointer[aansturing.blok].blokRicht[!aansturing.richtingVooruit] != &mBaanInfo->EindBlokPointer)
        {
            mMessage.message(str(boost::format
                                 ("Blok niet vrij %d at %d voor draaischijf %d") %
                                 aansturing.blok % EricFgetsGetLastLineCount ()% hardwareAdres));
            return WISSEL_ERR_INVALID_ADRES;
        }

        m_aansluitingen[aansturing.aansluitingNummer] = aansturing;
    }

    UpdateRec();
    if ((hardwareAdres < HARDWARE_MIN_ADRES) || (hardwareAdres >= MAX_NOBLOKS))
    {
        return WISSEL_ERR_INVALID_ADRES;
    }

    if (mBaanInfo->BlokPointer[hardwareAdres].BlokIONummer == -1)
    {
        // blok nog niet belegd door de draaischijf creer die
        mBaanInfo->BlokPointer[hardwareAdres].BlokIONummer = hardwareAdres;
        mBaanInfo->AantalBlokken += 1;
    }

    pBlok1 = &mBaanInfo->BlokPointer[hardwareAdres];
    pBlok1->pBlok->specialProcessing = [this]() { DraaiSchijfBlokAangesproken();};
    if (m_aansluitingen[0])
    {
        // aansluiting in gebruik dus initializeer de blok pointers
        mBaanInfo->BlokPointer[m_aansluitingen[0]->blok].blokRicht[!m_aansluitingen[0]->richtingVooruit] = pBlok1;
        pBlok1->blokRicht[m_aansluitingen[0]->richtingVooruit] = &mBaanInfo->BlokPointer[m_aansluitingen[0]->blok];
    }

    return 0;
}

void DraaiSchijf::DraaiSchijfBlokAangesproken()
{
    mBaanInfo->RegelArray[pBlok1->pBlok->RegelaarNummer].SetIgnoreStop(true);
    if ((pBlok1->pBlok->State == BLOK_VOORUIT) || (pBlok1->pBlok->State == BLOK_ACHTERUIT))
    {
        // enable midden detectie want er rijd een trein binnen
        Bedien(hardwareAdres+2,EnableMiddenDetectie, false);
    }

}

int DraaiSchijf::checkAansluiting(const DraaiSchijfAansluiting& aansturing)
{
    if ((aansturing.aansluitingNummer < 0) || (aansturing.aansluitingNummer >= 48))
    {
        mMessage.message(str(boost::format
                             ("Foutief aansluiting nummer %d at %d voor draaischijf %d") %
                             aansturing.aansluitingNummer % EricFgetsGetLastLineCount ()% hardwareAdres));
        return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
    }
    // check  of the baanblok aan die kant vrij is
    if (mBlok.BlokIsBlokNummer(aansturing.blok)==0)
    {
        mMessage.message(str(boost::format
                             ("Foutief blok nummer at %d voor draaischijf %d blok nummer %d") %
                             EricFgetsGetLastLineCount ()% hardwareAdres%
                              aansturing.blok));
        return WISSEL_ERR_INVALID_ADRES;
    }
    if (mBaanInfo->BlokPointer[aansturing.blok].blokRicht[!aansturing.richtingVooruit] != &mBaanInfo->EindBlokPointer)
    {
        mMessage.message(str(boost::format
                             ("Blok niet vrij %d at %d voor draaischijf %d") %
                             aansturing.blok % EricFgetsGetLastLineCount ()% hardwareAdres));
        return WISSEL_ERR_INVALID_ADRES;
    }
    return 0;
}

void DraaiSchijf::InitRoutering ()
{
    routeKnoopPunt[0].aansluitingen.push_back(1);
    routeKnoopPunt[0].aansluitingen.push_back(2);
    routeKnoopPunt[1].aansluitingen.push_back(0);
    routeKnoopPunt[2].aansluitingen.push_back(0);
//    if (Richting == 0)
//    {
//        BlokPointer_t * pBlok = pBlok1;
//        pBlok = pBlok->pVolgendBlok;
//        mWissels.VolgKnoopPunt(this, pBlok, 1);
//        pBlok = &StopBlokPointer[0];
//        pBlok = pBlok->pVolgendBlok;
//        mWissels.VolgKnoopPunt(this, pBlok, 2);
//    }
//    else
//    {
//        BlokPointer_t * pBlok = pBlok1;
//        mWissels.VolgKnoopPunt(this, pBlok, 0, this);
//    }
}

void DraaiSchijf::ZoekOngeinitializeerdeEindBlokken()
{
//    if (Richting == 0)
//    {
//        BlokPointer_t * pBlok = pBlok1;
//        mWissels.VolgKnoopPuntReverse(this, pBlok, 0, this);
//    }
//    else
//    {
//        BlokPointer_t * pBlok = pBlok1;
//        pBlok = pBlok->pVorigBlok;
//        mWissels.VolgKnoopPuntReverse(this, pBlok, 1);
//        pBlok = &StopBlokPointer[0];
//        pBlok = pBlok->pVorigBlok;
//        mWissels.VolgKnoopPuntReverse(this, pBlok, 2);
//    }
}

void DraaiSchijf::Display ()
{

    IMainWindowDrawing::BaanColor color;
    if (this == mBaanInfo->selectedWissel)
    {
        color = IMainWindowDrawing::Red;
    }
    else
    {
        color = IMainWindowDrawing::Black;
    }
    double angle = 0;
    if ((NieuweStand > 99) && (NieuweStand < 148))
    {
        angle = 7.5*(NieuweStand-100)+OffsetAngle;
    }
    if ((NieuweStand > 199) && (NieuweStand < 248))
    {
        angle = 7.5*(NieuweStand-200)+180+OffsetAngle;
    }
    angle = std::atan(1.0)*4.0 * angle/180.0;
    int xOffset = (int)(std::sin(angle) *Radius);
    int yOffset = (int)(std::cos(angle) *Radius);


    mMainWindowDrawing.DrawLine(Coord1X-xOffset,Coord1Y-yOffset, Coord1X+xOffset, Coord1Y+yOffset,5,color);
    mMainWindowDrawing.DrawLine(Coord1X+3*xOffset/4,Coord1Y+3*yOffset/4, Coord1X+xOffset, Coord1Y+yOffset,8,color);

}


void DraaiSchijf::WachtOp(int status)
{
    mThreadSleep.SleepFor(1);
    do
    {
        Bedien(hardwareAdres+2,GetStatus, true);
        mThreadSleep.SleepFor(1);
    }
    while ((hardwareReturnWaarde & status) && (mStopWorker ==false));
}

bool DraaiSchijf::GaNaarPositie(int positie)
{
    // eerst controleren of we wel kunnen draaien is de schijf vrij
    int stand = (Stand >=200) ? Stand-200: Stand - 100;
    if (pBlok1->pBlok->State != BLOK_VRIJ)
    {
        // er staat iets op de schijf en het heeft misschien nog snelheid
        if (pBlok1->pBlok->Snelheid != 0)
        {
            return true; // gewijgerd
        }
        if ((m_aansluitingen[stand]) && (pBlok1->blokRicht[m_aansluitingen[stand]->richtingVooruit]->pBlok->State != BLOK_VRIJ))
        {
            return true; // gewijgerd trein steekt nog uit in het aangesloten blok
        }
    }
    std::cout <<  "from " << Stand<< " to Pos " << positie << std::endl;
    // eerst de baan vakken omleggen
    pBlok1->blokRicht[0] = &mBaanInfo->EindBlokPointer;
    pBlok1->blokRicht[1] = &mBaanInfo->EindBlokPointer;
    if (m_aansluitingen[stand])
    {
        mBaanInfo->BlokPointer[m_aansluitingen[stand]->blok].blokRicht[!m_aansluitingen[stand]->richtingVooruit] = &mBaanInfo->EindBlokPointer;
    }
    stand = (positie >=200) ? positie-200: positie - 100;
    if (m_aansluitingen[stand])
    {
        mBaanInfo->BlokPointer[m_aansluitingen[stand]->blok].blokRicht[!m_aansluitingen[stand]->richtingVooruit] = pBlok1;
        pBlok1->blokRicht[m_aansluitingen[stand]->richtingVooruit] = &mBaanInfo->BlokPointer[m_aansluitingen[stand]->blok];
    }

    //m_aansluitingen[stand]->blok nog niet goed want die kan nu negatief zijn.
    // als die negatief is init relais nummer en laat hem wijzen naar een intern draaischijf blok.



    mWorker.Dispatch([=]()
    {
        Bedien(hardwareAdres+2,GetStatus, true);
        mThreadSleep.SleepFor(1);
        if (hardwareReturnWaarde & NeedsHoming)
        {
            Bedien(hardwareAdres+2,Homing);
            WachtOp(Turning);
        }

        auto bedienPos = (positie >=200) ? (positie-200+48/2)%48: positie - 100;
        Bedien(hardwareAdres + 2 + (positie >=200),bedienPos);
        WachtOp(Turning);
    });
    return false;
}

void DraaiSchijf::Bedien (int adres, int data, bool returnGewenst)
{
    hardwareArray_t bedien;
    bedien.blokIO = HW_IO;
    bedien.adres = adres;
    bedien.data = data; //hardwareBit;   // default IO_COMMAND0
    bedien.nummer = WisselNummer();
    bedien.returnGewenst = returnGewenst;

    if (mBaanInfo->hardwareHoog.nieuwItem (bedien))
    {
        mMessage.message (str(boost::format
                              ("hardware hoog vol info lost standaard wissel!")));
    }
}

int DraaiSchijf::Aanvraag (int stand)
{
    if (IOAANVRAAG_REFRESH == stand)
    {
//        Bedien ();
        NieuweStand = Stand; // initializatie van stand is buiten ons om gegaan dus refresh het
        return 0;
    }

    if (IOAANVRAAG_DEFAULT == stand)
    {
        stand = 100;
    }

    if (stand == IOAANVRAAG_TOGGLE)
    {
        if ((NieuweStand == Stand) && (StartDrag == false))
        {
            StartDrag = true;
        }
        else
        {
            // nu kan die gaan draaien
            StartDrag = false;
            if (GaNaarPositie(NieuweStand))
            {
                NieuweStand = Stand;
                mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);
                return IOGEWIJGERD;
            }
            Stand = NieuweStand;
        }
    }
    else if (stand & IOAANVRAAG_TOGGLE)
    {
        int x = (stand>>16) & 0x7fff;
        int y = stand &0x7fff;
        if (StartDrag)
        {
            int stand = (NieuweStand >= 200) ? (NieuweStand-200+48/2)%48: NieuweStand - 100;
            double angle = (stand*7.5+OffsetAngle)/180 * 4*std::atan(1.0);

            int xOffset = (int)(std::sin(angle) *Radius);
            int yOffset = (int)(std::cos(angle) *Radius);
            int length1 = SQR(x - (Coord1X+xOffset)) + SQR(y-(Coord1Y+yOffset));

            int length2 = SQR(x - (Coord1X-xOffset)) + SQR(y-(Coord1Y-yOffset));
            if (length1 < length2)
            {
                AndereKant = false;
            }
            else
            {
                AndereKant = true;
            }

            StartDrag = false;
        }
        int iSaved =0;
        int maxLength = 0x7fffffff;
        for (int s=0;s<48;s++)
        {
            int mult = (AndereKant) ? 1: 1;
            double angle = (s*7.5+OffsetAngle)/180 * 4*std::atan(1.0);
            int xOffset = (int)(std::sin(angle) *Radius);
            int yOffset = (int)(std::cos(angle) *Radius);
            int length = SQR(x - (Coord1X+mult*xOffset)) + SQR(y-(Coord1Y+mult*yOffset));
            if (length < maxLength)
            {
                iSaved = s;
                maxLength = length;
            }
        }
        NieuweStand = (AndereKant) ? iSaved + 200: iSaved+100;
    }
    else
    {
        // test of de stand binnen bereik ligt
        if (((stand < 100) || (stand > 247)) ||
                ((stand>147) && (stand <200)))
        {
            return IOGEWIJGERD;
        }
        NieuweStand = stand;
        if (GaNaarPositie(NieuweStand))
        {
            NieuweStand = Stand;
            mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);
            return IOGEWIJGERD;
        }
        Stand = NieuweStand;
    }

    mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);
    return 0;
}


std::string DraaiSchijf::String ()
{
    char string[200];
    std::string ret;

    std::vector<DraaiSchijfAansluiting> aansluitingen;
    for(auto i :m_aansluitingen )
    {
        if (i)
        {
            aansluitingen.emplace_back(*i);
        }
    }

    sprintf (string, "%d %7.2f %4d %4d %4d %4d %4d",
             Type,
             WisselAdres(),
             Coord1X,
             Coord1Y,
             Radius,
             OffsetAngle,
             static_cast<int>(aansluitingen.size())
             );
    ret = string;
    for (size_t i=0;i<aansluitingen.size();++i)
    {
        sprintf (string, "\n%4d %4d %4d %4d"
                 ,aansluitingen[i].aansluitingNummer
                 ,aansluitingen[i].gndFirst
                 ,aansluitingen[i].blok
                 ,aansluitingen[i].richtingVooruit);
        ret += string;
    }
    return ret;

}

void DraaiSchijf::NieuwXY (int selectionX,
                               int selectionY, int deltaX, int deltaY)
{
    if (mBaanInfo->selectedWisselPoint == -1)
    {
        if ((selectionX > (Coord1X-Radius/2)) && (selectionX < (Coord1X+Radius/2)) &&
            (selectionY > (Coord1Y-Radius/2)) && (selectionY < (Coord1Y+Radius/2)))
        {
            mBaanInfo->selectedWisselPoint = 1;  // midpoint
        }
        else
        {
            if ((selectionX > (Coord1X-Radius)) && (selectionX < (Coord1X+Radius)) &&
                (selectionY > (Coord1Y-Radius)) && (selectionY < (Coord1Y))) // top half
            {
                mBaanInfo->selectedWisselPoint = 2;  // radius update
            }

        }

    }

    switch (mBaanInfo->selectedWisselPoint)
    {
    case 1:
        Coord1X += deltaX;
        Coord1Y += deltaY;
        break;
    case 2:
        Radius += (deltaX+deltaY)/2;
        break;
    }
    mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());
    UpdateRec ();

    Display();
}

void DraaiSchijf::InitDialoog ()
{
    mDraaiWisselDialoog.SetUitleg(
                "DRAAISCHIJF.\n"
                "blok genomen\n");
    std::vector<DraaiSchijfAansluiting> aansluitingen;
    mDraaiWisselDialoog.SetAngle(OffsetAngle);
    mDraaiWisselDialoog.SetAngleToolTip("Offset hoek vanaf de default plek in graden");
    for(auto i :m_aansluitingen )
    {
        if (i)
        {
            aansluitingen.emplace_back(*i);
        }
    }
    mDraaiWisselDialoog.SetAdres(WisselAdres());

    mDraaiWisselDialoog.SetDraaiAansluitingen(aansluitingen);
}

void DraaiSchijf::DialoogOk ()
{

    hardwareAdres = static_cast<int>(mDraaiWisselDialoog.GetAdres());
    auto aansluitingen = mDraaiWisselDialoog.GetDraaiAansluitingen();
    for(auto& i :m_aansluitingen )
    {
        i = boost::none;
    }
    for(auto i:aansluitingen)
    {
        if (checkAansluiting(i) == 0)
        {
            m_aansluitingen[i.aansluitingNummer] = i;
        }
    }
    OffsetAngle = mDraaiWisselDialoog.GetAngle();
}
