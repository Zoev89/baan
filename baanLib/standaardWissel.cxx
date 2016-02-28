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
#include "standaardWissel.h"
#include "baanMessage.h"
#include "leesdata.h"
#include "blok.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/format.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StandaardWissel::StandaardWissel(IMessage &msg, IBlok& blok, IWissels &wissels, IBaanMessage &baanMessage, IMainWindowDrawing &mainWindowDrawing, IWisselDialoog &standaardWisselDialoog, BaanInfo_t *baanInfo, int kopBlok):
    IOBits_t(baanInfo->EindBlokPointer),
    mMessage(msg),
    mBlok(blok),
    mWissels(wissels),
    mBaanMessage(baanMessage),
    mMainWindowDrawing(mainWindowDrawing),
    mStandaardWisselDialoog(standaardWisselDialoog),
    mBaanInfo(baanInfo)
{
    pBlok1 = &mBaanInfo->BlokPointer[kopBlok];
    Stand = 13;
    rec.x (pBlok1->pBlok->XCoord + 40);
    rec.y (pBlok1->pBlok->YCoord);
    Coord1X = rec.x () + 2;
    Coord1Y = rec.b () - 2;
    Coord2X = rec.r () - 2;
    Coord2Y = rec.b () - 2;
    Coord3X = rec.r () - 2;
    Coord3Y = rec.y () + 2;
    Richting = 0;
    MaxSnelheid12 = -1;
    MaxSnelheid13 = -1;
    Lengte12 = -1;
    Lengte13 = -1;
    // default init naar eind blokken
    for (int i = 0; i < AANTAL_BLOKS_PER_WISSEL; i++)
    {
        StopBlokPointer[i].pVolgendBlok =
            &mBaanInfo->EindBlokPointer;
        StopBlokPointer[i].pVorigBlok =
            &mBaanInfo->EindBlokPointer;
        StopBlokPointer[i].pBlok = &(StopBlok[i]);
    }

}

StandaardWissel::~StandaardWissel()
{
    mBlok.BlokEndPointDelete (&StopBlokPointer[0], Richting);
}

BlokPointer_t * StandaardWissel::GetWisselBlok()
{
    return pBlok1;
}

int StandaardWissel::GetRichting()
{
    return Richting;
}

int StandaardWissel::WisselLengte(int from, int to)
{
    int lengte = - pBlok1->Lengte;
    switch(( from==0) ? to :  from)
    {
    case 1:
        lengte += abs(Lengte12);
        break;
    case 2:
        lengte += abs(Lengte13);
        break;
    }
    return lengte;
}


void StandaardWissel::VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok)
{
    if (pBlok1 == oudBlok)
    {
        pBlok1 = nieuwBlok;
    }
}

void StandaardWissel::TestIOTimer ()
{

}

float StandaardWissel::GetAdres()
{
    return mStandaardWisselDialoog.GetAdres();
}

int StandaardWissel::CheckWissel()
{
    if (CheckWisselBlok (pBlok1, Richting, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
        ("StandaardWissel %d.%02d aansluiting 2 niet goed aangesloten!") %
         hardwareAdres% hardwareBit));
        return 1;
    }
    if (CheckWisselBlok(&StopBlokPointer[0], Richting, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
        ("StandaardWissel %d.%02d aansluiting 3 niet goed aangesloten!") %
          hardwareAdres% hardwareBit));
        return 1;
    }
    return 0;
}

void StandaardWissel::UpdateRec ()
{
    int i;

    i = Coord1X;
    if (Coord2X < i)
        i = Coord2X;
    if (Coord3X < i)
        i = Coord3X;
    rec.x (i - 3);
    i = Coord1Y;
    if (Coord2Y < i)
        i = Coord2Y;
    if (Coord3Y < i)
        i = Coord3Y;
    rec.y (i - 3);
    i = Coord1X;
    if (Coord2X > i)
        i = Coord2X;
    if (Coord3X > i)
        i = Coord3X;
    rec.w (i - rec.x () + 3);
    i = Coord1Y;
    if (Coord2Y > i)
        i = Coord2Y;
    if (Coord3Y > i)
        i = Coord3Y;
    rec.h (i - rec.y () + 3);
}


int StandaardWissel::Init (char *Input, FILE *)
{
    float floatAdres;
    int Blok1;
    char Blok3Type[2];
    float adres3;
    BlokPointer_t *pBlok3;

    /* Lees alle velden in */
    if (sscanf (Input, "%d%f%d%d%d%d%d%d%d%1s%f%d%d%d%d",
                &Type,
                &floatAdres,
                &Coord1X,
                &Coord1Y,
                &Coord2X,
                &Coord2Y,
                &Coord3X,
                &Coord3Y,
                &Blok1,
                Blok3Type,
                &adres3,
                &Lengte12,
                &Lengte13,
                &MaxSnelheid12,
                &MaxSnelheid13) != 15)
        return WISSEL_ERR_NIET_ALLES_AANWEZIG;

    UpdateRec();


    if ((Blok1 < HARDWARE_MIN_ADRES) || (Blok1 >= MAX_NOBLOKS))
    {
        return WISSEL_ERR_INVALID_ADRES;
    }
    if (mBaanInfo->BlokPointer[Blok1].BlokIONummer == -1)
    {
        return WISSEL_ERR_INVALID_ADRES;
    }

    pBlok1 = &mBaanInfo->BlokPointer[Blok1];


    pBlok3 = mWissels.wisselKrijgPointer (Blok3Type[0], adres3);
    if (pBlok3 == NULL)
        return WISSEL_ERR_INVALID_ADRES;

    if (-1 == adres3)
    {
        Richting = 1;
    }
    else
    {
        Richting = 0;
    }


    Stand = 12;          /* default wijzen we naar 2 */

    /* Controleer of de maximum snelheid gebruikt wordt */
    if (MaxSnelheid12 >= 0)
    {
        /* omdat deze maximum snelheid gebruikt wordt zet hem op de baan */
        pBlok1->pBlok->MaxSnelheid =
                MaxSnelheid12;
    }


    /* stand is 12 dus init baan lengte */
    if (Lengte12 >= 0)
        pBlok1->Lengte = Lengte12;


    if (Richting == 0)
    {
        /* de richting is voorwaards */
        if (&mBaanInfo->EindBlokPointer != StopBlokPointer[0].pVolgendBlok)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format
                                 ("Regel %d: Van Wissel %d.%02d aansluiting 3 is al aangesloten met blok %d\n%s\nRichting misschien fout of aansluiting 2 met 3 verwisseld?")%
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                 hardwareBit%
                                 StopBlokPointer[0].pVolgendBlok->BlokIONummer% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }

        if (&mBaanInfo->EindBlokPointer != pBlok3->pVorigBlok)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format
                                 ("Regel %d: Van Wissel %d.%02d aansluiting 3 is het volgend blok zijn TERUG weg al belegd met blok %d\n%s") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                 hardwareBit% pBlok3->pVorigBlok->BlokIONummer% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }
        StopBlokPointer[0].pVolgendBlok = pBlok3;
        pBlok3->pVorigBlok = &StopBlokPointer[0];
    }
    else
    {
        /* de richting is achteruit */
        // niets te doen de andere doen dit.
        // maar aansluiting 2 moet er al zijn dus die controleren we
        if (pBlok1->pVolgendBlok == &mBaanInfo->EindBlokPointer)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format
                                 ("Regel %d: Van Wissel %d.%02d aansluiting 2 is nog niet aangesloten dit had in de blok sectie al gedaan moeten zijn\n%s") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                 hardwareBit% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }

    }
    return 0;
}


void StandaardWissel::InitRoutering ()
{
    routeKnoopPunt[0].aansluitingen.push_back(1);
    routeKnoopPunt[0].aansluitingen.push_back(2);
    routeKnoopPunt[1].aansluitingen.push_back(0);
    routeKnoopPunt[2].aansluitingen.push_back(0);
    if (Richting == 0)
    {
        BlokPointer_t * pBlok = pBlok1;
        pBlok = pBlok->pVolgendBlok;
        mWissels.VolgKnoopPunt(this, pBlok, 1);
        pBlok = &StopBlokPointer[0];
        pBlok = pBlok->pVolgendBlok;
        mWissels.VolgKnoopPunt(this, pBlok, 2);
    }
    else
    {
        BlokPointer_t * pBlok = pBlok1;
        mWissels.VolgKnoopPunt(this, pBlok, 0, this);
    }
}

void StandaardWissel::ZoekOngeinitializeerdeEindBlokken()
{
    if (Richting == 0)
    {
        BlokPointer_t * pBlok = pBlok1;
        mWissels.VolgKnoopPuntReverse(this, pBlok, 0, this);
    }
    else
    {
        BlokPointer_t * pBlok = pBlok1;
        pBlok = pBlok->pVorigBlok;
        mWissels.VolgKnoopPuntReverse(this, pBlok, 1);
        pBlok = &StopBlokPointer[0];
        pBlok = pBlok->pVorigBlok;
        mWissels.VolgKnoopPuntReverse(this, pBlok, 2);
    }
}

void StandaardWissel::Display ()
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

    if (Stand == 12)
    {
        mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
                                    Coord2X, Coord2Y,
                                    5, color);
    }
    else
    {
        mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
                                    Coord3X, Coord3Y,
                                    5, color);
    }
}

void StandaardWissel::Bedien ()
{
    hardwareArray_t bedien;

    bedien.blokIO = HW_IO;
    bedien.adres = hardwareAdres;
    bedien.data = hardwareBit;   // default bitNummer + IO_COMMAND0
    bedien.nummer = 0;
    bedien.returnGewenst = 0;

    if (Stand == 13)
    {
        // wissel gebogen
        bedien.data |= IO_COMMAND3;
    }
    if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
    {
        mMessage.message (str(boost::format
                              ("hardware hoog vol info lost standaard wissel!")));
    }
}

int StandaardWissel::Aanvraag (int stand)
{
    BlokPointer_t *pBlok2;
    BlokPointer_t *pBlok3;
    int richting;
    int tegenRichting;
    BlokPointer_t *pStopBlok;

    if (IOAANVRAAG_REFRESH == stand)
    {
        Bedien ();
        return 0;
    }
    if (IOAANVRAAG_DEFAULT == stand)
    {
        stand = 12;
    }
    if (Stand == stand)
    {
        // als de stand gelijk is dan meteen terug alsof die bedient is
        return 0;
    }

    if (pBlok1->pBlok->State != BLOK_VRIJ)
    {
        return IOGEWIJGERD;
    }
    richting = Richting;
    tegenRichting = (richting + 1) & 1;

    pBlok2 = pBlok1->blokRicht[richting];


#ifdef ZET_CHECKING_UIT
    if (((pBlok2->pBlok->State & BLOK_BEZET_MASK) != BLOK_VRIJ) &&
            (pBlok2->pBlok->Snelheid != 0))
    {
        return IOGEWIJGERD;       /* Wissel is bezet dus niet omschakelen */
    }
#endif

    pStopBlok = &StopBlokPointer[0];

    pBlok3 = pStopBlok->blokRicht[richting];

    pBlok1->blokRicht[richting] = pBlok3;
    pBlok3->blokRicht[tegenRichting] = pBlok1;
    pBlok2->blokRicht[tegenRichting] = pStopBlok;
    pStopBlok->blokRicht[richting] = pBlok2;


    if (Stand == 12)
    {
        /* De wissel staat op 12 */

        Stand = 13;
        /* Zet de maximum snelheid op */
        if (MaxSnelheid13 >= 0)
            pBlok1->pBlok->MaxSnelheid = MaxSnelheid13;

        /* Zet de lengte info */
        if (Lengte13 >= 0)
            pBlok1->Lengte = Lengte13;

    }
    else
    {
        Stand = 12;
        /* Zet de maximum snelheid op */
        if (MaxSnelheid12 >= 0)
            pBlok1->pBlok->MaxSnelheid = MaxSnelheid12;

        /* Zet de lengte info */
        if (Lengte12 >= 0)
            pBlok1->Lengte = Lengte12;
    }

    Bedien ();
    mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);


    return 0;
}

// TODO remove this when mBaanMessage.Post supports a std::function
int StandaardWissel::WisselNummer()
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

void StandaardWissel::String (char *string)
{
    char sString[20];
    char bString[20];

    if (Richting)
    {
        // richting terug
        strcpy (sString, "B-1");
    }
    else
    {
        mBlok.BlokNaam (sString, StopBlokPointer[0].pVolgendBlok);
    }
    mBlok.BlokNaam (bString, pBlok1);

    sprintf (string, "%d %7.2f %4d %4d %4d %4d %4d %4d %8s %8s %4d %4d %4d %4d",
             Type,
             WisselAdres(),
             Coord1X,
             Coord1Y,
             Coord2X,
             Coord2Y,
             Coord3X,
             Coord3Y,
             &bString[1],
            sString,
            Lengte12,
            Lengte13,
            MaxSnelheid12, MaxSnelheid13);
}

void StandaardWissel::NieuwXY (int selectionX,
                               int selectionY, int deltaX, int deltaY)
{
    if (mBaanInfo->selectedWisselPoint == -1)
    {
        int afstand1, afstand2, x, y;
        afstand1 = SQR (Coord1X - selectionX) +
                SQR (Coord1Y - selectionY);
        if (Stand == 12)
        {
            mBaanInfo->selectedWisselPoint = 2;
            x = Coord2X;
            y = Coord2Y;
        }
        else
        {
            mBaanInfo->selectedWisselPoint = 3;
            x = Coord3X;
            y = Coord3Y;
        }
        afstand2 = SQR (x - selectionX) + SQR (y - selectionY);
        if (afstand1 < afstand2)
        {
            afstand2 = afstand1;
            mBaanInfo->selectedWisselPoint = 1;
        }
        x = rec.center_x ();
        y = rec.center_y ();
        afstand1 = SQR (x - selectionX) + SQR (y - selectionY);
        if (afstand1 < afstand2)
        {
            mBaanInfo->selectedWisselPoint = 4;
        }

    }

    switch (mBaanInfo->selectedWisselPoint)
    {
    case 1:
        Coord1X += deltaX;
        Coord1Y += deltaY;
        break;
    case 2:
        Coord2X += deltaX;
        Coord2Y += deltaY;
        break;
    case 3:
        Coord3X += deltaX;
        Coord3Y += deltaY;
        break;
    case 4:
        Coord1X += deltaX;
        Coord1Y += deltaY;
        Coord2X += deltaX;
        Coord2Y += deltaY;
        Coord3X += deltaX;
        Coord3Y += deltaY;
        break;
    }
    mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());
    UpdateRec ();

    Display();
}

void StandaardWissel::InitDialoog ()
{
    char string[20];

    mStandaardWisselDialoog.SetUitleg(
                "12 is de rechte kant en 13 is buigend.\n"
                "Een gebogen wissel 12 is de grote bocht en 13 is\n"
                "de krappe bocht.\n"
                "Een wissel kan op twee manieren liggen namelijk:\n"
                "  Voorwaards         Achterwaards\n"
                "       /----2--    --3----\n"
                " -1---/                    /---1-\n"
                "        ----3--    --2----/\n"
                "Een voorwaardse wissel 3 is het volgende blok/wissel.\n"
                "Een achterwaardse wissel 3 is B-1 deze wordt dan door\n"
                "een andere wissel of baanvak ingevult (Wxxxx.yy)\n"
                "Bij een negatieve lengte wordt de lengte van het\n"
                "blok genomen\n");
    mStandaardWisselDialoog.SetAdres(WisselAdres());

    if (Richting)
    {
        sprintf (string, "B-1");
    }
    else
    {
        mBlok.BlokNaam (string, StopBlokPointer[0].blokRicht[0]);
    }
    mStandaardWisselDialoog.SetAansluiting(1, string);
    mStandaardWisselDialoog.SetAansluitingToolTip(1, "Stand 3 gebruik de B/W/w notatie geef B-1 voor een wissel in tegen richting.");
    mStandaardWisselDialoog.SetLengte(0,Lengte12);
    mStandaardWisselDialoog.SetLengteToolTip(0, "De lengte in stand 12. Geef -1 als de lengte van blok 1 correct is.");
    mStandaardWisselDialoog.SetLengte(1, Lengte13);
    mStandaardWisselDialoog.SetLengteToolTip(1, "De lengte in stand 13. Geef -1 als de lengte van blok 1 correct is.");
    mStandaardWisselDialoog.SetMaxSnelheid(0, MaxSnelheid12);
    mStandaardWisselDialoog.SetMaxSnelheidToolTip(0, "Geef de snelheid limitatie voor 12 stand. Geef -1 voor geen limiet.");
    mStandaardWisselDialoog.SetMaxSnelheid(1, MaxSnelheid13);
    mStandaardWisselDialoog.SetMaxSnelheidToolTip(1, "Geef de snelheid limitatie voor 13 stand. Geef -1 voor geen limiet.");
}

void StandaardWissel::DialoogOk ()
{
    BlokPointer_t *wisselBlok;
    char blokType[2];
    float adres;
    int richting;

    Lengte12 = mStandaardWisselDialoog.GetLengte(0);
    Lengte13 = mStandaardWisselDialoog.GetLengte(1);
    MaxSnelheid12 = mStandaardWisselDialoog.GetMaxSnelheid(0);
    MaxSnelheid13 = mStandaardWisselDialoog.GetMaxSnelheid(1);


    // volgend blok gewijzigd
    if (sscanf
            (mStandaardWisselDialoog.GetAansluiting(1).c_str(), "%1s%f", blokType, &adres) == 2)
    {
        wisselBlok = mWissels.wisselKrijgPointer (blokType[0], adres);
        if (wisselBlok == NULL)
        {
            mMessage.message(str(boost::format
                                 ("Het volgend blok bestaat niet %s") %
                                 mStandaardWisselDialoog.GetAansluiting(1).c_str()));
            // wijziging ongedaan gemaakt
        }
        else if (wisselBlok == pBlok1)
        {
            mMessage.message(str(boost::format
                                 ("Het volgend blok %s is hetzelfde als het wissel blok wijzing niet gedaan")%
                                 mStandaardWisselDialoog.GetAansluiting(1).c_str()));
        }
        else
        {
            // wisselBlok wijst naar een blok
            if (wisselBlok == &mBaanInfo->EindBlokPointer)
            {
                richting = 1;
            }
            else
            {
                richting = 0;
            }
            if ((richting != Richting) || (richting == 0))
            {

                mBlok.BlokEndPointDelete (&(StopBlokPointer[0]),
                        Richting);

                StopBlokPointer[0].blokRicht[richting] = wisselBlok;
                mBlok.BlokEndPointInsert (&(StopBlokPointer[0]), richting);
                Richting = richting;
            }

        }
    }
    else
    {
        mMessage.message(str(boost::format
                             ("Blok %s op aansluiting 3 voldoet niet aan B/W/w notatie.") %
                             mStandaardWisselDialoog.GetAansluiting(1).c_str()));
    }
}
