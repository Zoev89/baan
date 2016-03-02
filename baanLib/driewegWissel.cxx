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
#include "driewegWissel.h"
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

DriewegWissel::DriewegWissel(IMessage &msg, IBlok& blok, IWissels& wissels, IBaanMessage &baanMessage, IMainWindowDrawing &mainWindowDrawing, IWisselDialoog &driewegWisselDialoog, BaanInfo_t *baanInfo, int kopBlok):
    mMessage(msg),
    mBlok(blok),
    mWissels(wissels),
    mBaanMessage(baanMessage),
    mMainWindowDrawing(mainWindowDrawing),
    mDriewegWisselDialoog(driewegWisselDialoog),
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
    Coord4X = rec.x () + 2;
    Coord4Y = rec.y () + 2;
    Richting = 0;
    MaxSnelheid12 = -1;
    MaxSnelheid13 = -1;
    MaxSnelheid14 = -1;
    Lengte12 = -1;
    Lengte13 = -1;
    Lengte14 = -1;
    StopBlok.resize(3);
    StopBlokPointer.resize(3);
    routeKnoopPunt.resize(4);
    IOBits_t::InitConstructor(baanInfo->EindBlokPointer);
}

DriewegWissel::~DriewegWissel()
{
    mBlok.BlokEndPointDelete (&StopBlokPointer[0], Richting);
    mBlok. BlokEndPointDelete (&StopBlokPointer[2], Richting);
}

BlokPointer_t * DriewegWissel::GetWisselBlok()
{
    return pBlok1;
}

int DriewegWissel::GetRichting()
{
    return Richting;
}

int DriewegWissel::WisselLengte(int from, int to)
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
    case 3:
        lengte += abs(Lengte14);
        break;
    }
    return lengte;
}


void DriewegWissel::VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok)
{
    if (pBlok1 == oudBlok)
    {
        pBlok1 = nieuwBlok;
    }
}

int DriewegWissel::CheckWissel()
{
    if (CheckWisselBlok (pBlok1, Richting, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
        ("DriewegWissel %d.%02d aansluiting 3 niet goed aangesloten!") %
                             hardwareAdres% hardwareBit));
        return 1;
    }
    if (CheckWisselBlok
            (&StopBlokPointer[0], Richting, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
        ("DriewegWissel %d.%02d aansluiting 2 niet goed aangesloten!") %
                             hardwareAdres% hardwareBit));
        return 1;
    }
    if (CheckWisselBlok
            (&StopBlokPointer[2], Richting, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
        ("DriewegWissel %d.%02d aansluiting 4 niet goed aangesloten!")%
                             hardwareAdres% hardwareBit));
        return 1;
    }
    return 0;
}

void DriewegWissel::TestIOTimer ()
{

}

float DriewegWissel::GetAdres()
{
    return mDriewegWisselDialoog.GetAdres();
}

void DriewegWissel::UpdateRec ()
{
    int i;

    i = (Coord1X);
    if (Coord2X < i)
        i = Coord2X;
    if (Coord3X < i)
        i = Coord3X;
    if (Coord4X < i)
        i = Coord4X;
    rec.x (i - 3);
    i = (Coord1Y);
    if (Coord2Y < i)
        i = Coord2Y;
    if (Coord3Y < i)
        i = Coord3Y;
    if (Coord4Y < i)
        i = Coord4Y;
    rec.y (i - 3);
    i = (Coord1X);
    if (Coord2X > i)
        i = Coord2X;
    if (Coord3X > i)
        i = Coord3X;
    if (Coord4X > i)
        i = Coord4X;
    rec.w (i - rec.x () + 3);
    i = (Coord1Y);
    if (Coord2Y > i)
        i = Coord2Y;
    if (Coord3Y > i)
        i = Coord3Y;
    if (Coord4Y > i)
        i = Coord4Y;
    rec.h (i - rec.y () + 3);
}


int DriewegWissel::Init (const char *Input, std::function<std::string()> )
{
    /* Deze init routine belegt gebruikt voor:
   ** 2  spoel + 0
   ** 3  spoel + 1
   ** 4  spoel + 2
   ** Dus als een wissel wil aansluiten op 4 dan moet die w inplaats van W
   ** geven en dan nemen we het stopblok van spoel + 2
   **/
    float floatAdres;
    int Blok1;
    char Blok2Type[2];
    char Blok4Type[2];
    float adres2;
    float adres4;

    BlokPointer_t *pBlok2, *pBlok4;

    /* Lees alle velden in */
    if (sscanf
            (Input, "%d%f%d%d%d%d%d%d%d%d%d%1s%f%1s%f%d%d%d%d%d%d",
             &Type, &floatAdres,
             &Coord1X,
             &Coord1Y,
             &Coord2X,
             &Coord2Y,
             &Coord3X,
             &Coord3Y,
             &Coord4X,
             &Coord4Y, &Blok1, Blok2Type, &adres2,
             Blok4Type, &adres4,
             &Lengte12,
             &Lengte13,
             &Lengte14,
             &MaxSnelheid12,
             &MaxSnelheid13, &MaxSnelheid14) != 21)
        return WISSEL_ERR_NIET_ALLES_AANWEZIG;

    if (adres2 == -1)
    {
        Richting = 1;
    }
    else
    {
        Richting = 0;
    }

    if ((Blok1 < HARDWARE_MIN_ADRES) || (Blok1 >= MAX_NOBLOKS))
    {
        return WISSEL_ERR_INVALID_ADRES;
    }
    if (mBaanInfo->BlokPointer[Blok1].BlokIONummer == -1)
    {
        return WISSEL_ERR_INVALID_ADRES;
    }
    pBlok1 = &mBaanInfo->BlokPointer[Blok1];

    pBlok2 = mWissels.wisselKrijgPointer (Blok2Type[0], adres2);
    if (pBlok2 == NULL)
        return WISSEL_ERR_INVALID_ADRES;
    pBlok4 = mWissels.wisselKrijgPointer (Blok4Type[0], adres4);
    if (pBlok4 == NULL)
        return WISSEL_ERR_INVALID_ADRES;

    Stand = 13;          /* default wijzen we naar 3 */

    /* Controleer of de maximum snelheid gebruikt wordt */
    if (MaxSnelheid13 >= 0)
    {
        /* omdat deze maximum snelheid gebruikt wordt zet hem op de baan */
        pBlok1->pBlok->MaxSnelheid =
                MaxSnelheid13;
    }


    /* stand is 13 dus init baan lengte */
    if (Lengte13 >= 0)
        pBlok1->Lengte = Lengte13;

    if (Richting == 0)
    {
        /* de richting is voorwaards */
        // check of 2 vrij is
        if (&mBaanInfo->EindBlokPointer != StopBlokPointer[0].pVolgendBlok)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format(
                                     "Regel %d: Van Wissel %d.%02d aansluiting 2 is al aangesloten met blok %d\n%s\nRichting misschien fout of aansluiting 2 met 3,4 verwisseld?") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres %
                                 hardwareBit%
                                 StopBlokPointer[0].pVolgendBlok->BlokIONummer% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }

        if (&mBaanInfo->EindBlokPointer != pBlok2->pVorigBlok)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format(
                                     "Regel %d: Van Wissel %d.%02d aansluiting 2 is het volgend blok zijn TERUG weg al belegd met blok %d\n%s") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                 hardwareBit% pBlok2->pVorigBlok->BlokIONummer% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }

        // check of 4 vrij is
        if (&mBaanInfo->EindBlokPointer != StopBlokPointer[2].pVolgendBlok)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format(
                                     "Regel %d: Van Wissel %d.%02d aansluiting 4 is al aangesloten met blok %d\n%s\nRichting misschien fout of aansluiting 2 met 3,4 verwisseld?") %
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                 hardwareBit%
                                 StopBlokPointer[2].pVolgendBlok->BlokIONummer% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }

        if (&mBaanInfo->EindBlokPointer != pBlok4->pVorigBlok)
        {
            // we hebben een probleem
            mMessage.message (str(boost::format(
                                      "Regel %d: Van Wissel %d.%02d aansluiting 4 is het volgend blok zijn TERUG weg al belegd met blok %d\n%s") %
                                  EricFgetsGetLastLineCount ()% hardwareAdres%
                                  hardwareBit% pBlok2->pVorigBlok->BlokIONummer% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }
        StopBlokPointer[0].pVolgendBlok = pBlok2;
        StopBlokPointer[1].pVolgendBlok =
                pBlok1->pVolgendBlok;
        StopBlokPointer[2].pVolgendBlok = pBlok4;
        pBlok2->pVorigBlok = &StopBlokPointer[0];
        pBlok4->pVorigBlok = &StopBlokPointer[2];
    }
    else
    {
        /* de richting is achteruit */
        StopBlokPointer[1].pVorigBlok =
                pBlok1->pVorigBlok;
        // maar aansluiting 2 moet er al zijn dus die controleren we
        if (pBlok1->pVolgendBlok == &mBaanInfo->EindBlokPointer)
        {
            // we hebben een probleem
            mMessage.message(str(boost::format
                                 ("Regel %d: Van Wissel %d.%02d aansluiting 3 is nog niet aangesloten dit had in de blok sectie al gedaan moeten zijn\n%s")%
                                 EricFgetsGetLastLineCount ()% hardwareAdres%
                                 hardwareBit% Input));
            return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
        }
    }

    UpdateRec ();
    return 0;

}

void DriewegWissel::InitRoutering ()
{
    routeKnoopPunt[0].aansluitingen.push_back(2);
    routeKnoopPunt[0].aansluitingen.push_back(1);
    routeKnoopPunt[0].aansluitingen.push_back(3);
    routeKnoopPunt[1].aansluitingen.push_back(0);
    routeKnoopPunt[2].aansluitingen.push_back(0);
    routeKnoopPunt[3].aansluitingen.push_back(0);
    if (Richting == 0)
    {
        BlokPointer_t * pBlok = pBlok1;
        pBlok = pBlok->pVolgendBlok;
        mWissels.VolgKnoopPunt(this, pBlok, 2);
        pBlok = &StopBlokPointer[0];
        pBlok = pBlok->pVolgendBlok;
        mWissels.VolgKnoopPunt(this, pBlok, 1);
        pBlok = &StopBlokPointer[2];
        pBlok = pBlok->pVolgendBlok;
        mWissels.VolgKnoopPunt(this, pBlok, 3);
    }
    else
    {
        BlokPointer_t * pBlok = pBlok1;
        mWissels.VolgKnoopPunt(this, pBlok, 0, this);
    }
}

void DriewegWissel::ZoekOngeinitializeerdeEindBlokken()
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
        mWissels.VolgKnoopPuntReverse(this, pBlok, 2);
        pBlok = &StopBlokPointer[0];
        pBlok = pBlok->pVorigBlok;
        mWissels.VolgKnoopPuntReverse(this, pBlok, 1);
        pBlok = &StopBlokPointer[2];
        pBlok = pBlok->pVorigBlok;
        mWissels.VolgKnoopPuntReverse(this, pBlok, 3);
    }
}


void DriewegWissel::Display ()
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
        if (Stand == 13)
        {
            mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
                                        Coord3X, Coord3Y,
                                        5, color);
        }
        else
        {
            mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
                                        Coord4X, Coord4Y,
                                        5, color);
        }
    }
}

void DriewegWissel::Bedien ()
{
    hardwareArray_t bedien;


    bedien.blokIO = HW_IO;
    bedien.adres = hardwareAdres;
    bedien.data = hardwareBit;   // default IO_COMMAND0
    bedien.nummer = 0;
    bedien.returnGewenst = 0;

    if (Stand == 12)
    {
        // wissel gebogen
        bedien.data |= IO_COMMAND3;
    }
    if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
    {
        mMessage.message ("hardware hoog vol info lost drieweg wissel!");
    }

    bedien.data = hardwareBit + 1;       // default IO_COMMAND0
    if (Stand == 14)
    {
        // wissel gebogen
        bedien.data |= IO_COMMAND3;
    }
    if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
    {
        mMessage.message ("hardware hoog vol info lost drieweg wissel!");
    }
}

int DriewegWissel::Aanvraag (int stand)
{
    int NieuweStand;
    BlokPointer_t *pBlokOld, *pBlokOldStop, *pBlokNieuw,
            *pBlokNieuwStop;
    int RegelaarNummer;
    int MaxSnelheid;
    int Lengte;
    int richting;
    int tegenRichting;

    if (IOAANVRAAG_REFRESH == stand)
    {
        Bedien ();
        return 0;
    }

    if (IOAANVRAAG_DEFAULT == stand)
    {
        stand = 13;
    }


    pBlok1 = pBlok1;
    if ((pBlok1->pBlok->State != BLOK_VRIJ) && (pBlok1->pBlok->Snelheid != 0))
    {
        return IOGEWIJGERD;
    }

    richting = Richting;
    tegenRichting = (richting + 1) & 1;

    // TODO de standaardWissel geeft eventueel het check blok vrij
    // Deze doet dat nog niet
    RegelaarNummer = pBlok1->pBlok->RegelaarNummer;


    /* de richting is voorwaards */

    pBlokOld = pBlok1->blokRicht[richting];

    if ((pBlok1->pBlok->State != BLOK_VRIJ) &&
            (RegelaarNummer == pBlokOld->pBlok->RegelaarNummer))
    {
        return IOGEWIJGERD;       /* wissel + huidig blok zijn bezet met dezelfde regelaar */
    }

    if (((pBlokOld->pBlok->State & BLOK_BEZET_MASK) != BLOK_VRIJ) &&
            (pBlokOld->pBlok->Snelheid != 0))
    {
        return IOGEWIJGERD;       /* Wissel is bezet dus niet omschakelen */
    }

    if (stand == IOAANVRAAG_TOGGLE)
    {
        NieuweStand = Stand + 1;
        if (NieuweStand > 14)
        {
            NieuweStand = 12;
        }
    }
    else
    {
        // test of de stand binnen bereik ligt
        if ((stand < 12) || (stand > 14))
            return IOGEWIJGERD;
        NieuweStand = stand;
    }

    if (NieuweStand == Stand)
        return 0;                   /* Zelfde stand gewenst */

    switch (Stand)
    {
    case 12:
        pBlokOldStop = &StopBlokPointer[0];
        break;
    case 13:
        pBlokOldStop = &StopBlokPointer[1];
        break;
    case 14:
        pBlokOldStop = &StopBlokPointer[2];
        break;
    }
    /* Richting is vooruit */
    switch (NieuweStand)
    {
    case 12:
        pBlokNieuwStop = &StopBlokPointer[0];
        break;
    case 13:
        pBlokNieuwStop = &StopBlokPointer[1];
        break;
    case 14:
        pBlokNieuwStop = &StopBlokPointer[2];
        break;
    }
    pBlokNieuw = pBlokNieuwStop->blokRicht[richting];


    pBlok1->blokRicht[richting] = pBlokNieuw;
    pBlokNieuw->blokRicht[tegenRichting] = pBlok1;
    pBlokOld->blokRicht[tegenRichting] = pBlokOldStop;
    pBlokOldStop->blokRicht[richting] = pBlokOld;

    Stand = NieuweStand;

    switch (NieuweStand)
    {
    case 12:
        MaxSnelheid = MaxSnelheid12;
        Lengte = Lengte12;
        break;
    case 13:
        MaxSnelheid = MaxSnelheid13;
        Lengte = Lengte13;
        break;
    case 14:
        MaxSnelheid = MaxSnelheid14;
        Lengte = Lengte14;
        break;
    }
    if (MaxSnelheid >= 0)
        pBlok1->pBlok->MaxSnelheid = MaxSnelheid;

    /* Zet de lengte info */
    if (Lengte >= 0)
        pBlok1->Lengte = Lengte;

    Bedien ();
    mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);
    return 0;
}

// TODO remove this when mBaanMessage.Post supports a std::function
int DriewegWissel::WisselNummer()
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


void DriewegWissel::String (char *string)
{
    char s1String[20];
    char s2String[20];
    char bString[20];

    if (Richting)
    {
        // richting terug
        strcpy (s1String, "B-1");
        strcpy (s2String, "B-1");
    }
    else
    {
        mBlok.BlokNaam (s1String, StopBlokPointer[0].pVolgendBlok);
        mBlok.BlokNaam (s2String, StopBlokPointer[2].pVolgendBlok);
    }
    mBlok.BlokNaam (bString, pBlok1);

    sprintf (string,
             "%d %7.2f %4d %4d %4d %4d %4d %4d %4d %4d %8s %8s %8s %4d %4d %4d %4d %4d %4d",
             Type,
             WisselAdres(),
             Coord1X,
             Coord1Y,
             Coord2X,
             Coord2Y,
             Coord3X,
             Coord3Y,
             Coord4X,
             Coord4Y,
             &bString[1],
            s1String,
            s2String,
            Lengte12,
            Lengte13,
            Lengte14,
            MaxSnelheid12,
            MaxSnelheid13, MaxSnelheid14);

}

void DriewegWissel::NieuwXY (int selectionX,
                             int selectionY, int deltaX, int deltaY)
{
    if (mBaanInfo->selectedWisselPoint == -1)
    {
        int afstand1, afstand2, x, y;
        afstand1 = SQR (Coord1X - selectionX) +
                SQR (Coord1Y - selectionY);
        switch (Stand)
        {
        case 12:
            mBaanInfo->selectedWisselPoint = 2;
            x = Coord2X;
            y = Coord2Y;
            break;
        case 13:
            mBaanInfo->selectedWisselPoint = 3;
            x = Coord3X;
            y = Coord3Y;
            break;
        case 14:
            mBaanInfo->selectedWisselPoint = 4;
            x = Coord4X;
            y = Coord4Y;
            break;
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
            mBaanInfo->selectedWisselPoint = 5;
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
        Coord4X += deltaX;
        Coord4Y += deltaY;
        break;
    case 5:
        Coord1X += deltaX;
        Coord1Y += deltaY;
        Coord2X += deltaX;
        Coord2Y += deltaY;
        Coord3X += deltaX;
        Coord3Y += deltaY;
        Coord4X += deltaX;
        Coord4Y += deltaY;
        break;
    }
    mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());
    UpdateRec ();

    Display ();
}

void DriewegWissel::InitDialoog ()
{
    char string[20];

    mDriewegWisselDialoog.SetUitleg(
                "13 is de rechte kant\n"
                "Een wissel kan op twee manieren liggen namelijk:\n"
                "  Voorwaards         Achterwaards\n"
                "       /----2--      --4----\n"
                " -1---/  ---3--      --3---  /---1-\n"
                "        ----4--      --2----/\n"
                "13 word door de blok database gedaan.\n"
                "Voorwaardse wissel 2 en 4 is het volgende blok/wissel.\n"
                "Achterwaardse wissel 2 en 4 is B-1 deze wordt dan\n"
                "door een andere wissel of baanvak ingevult\n"
                "2=Wxxxx.yy en 4=wxxxx.yy\n"
                "Bij een negatieve lengte wordt de lengte van het\n"
                "blok genomen.\n");
    mDriewegWisselDialoog.SetAdres(WisselAdres());

    if (Richting)
    {
        sprintf (string, "B-1");
    }
    else
    {
        mBlok.BlokNaam (string, StopBlokPointer[0].blokRicht[0]);
    }
    mDriewegWisselDialoog.SetAansluiting(0, string);
    mDriewegWisselDialoog.SetAansluitingToolTip(0, "Aansluting 2 gebruik de B/W/w notatie geef B-1 voor een wissel in tegen richting.");

    if (Richting)
    {
        sprintf (string, "B-1");
    }
    else
    {
        mBlok.BlokNaam (string, StopBlokPointer[2].blokRicht[0]);
    }
    mDriewegWisselDialoog.SetAansluiting(2,string);
    mDriewegWisselDialoog.SetAansluitingToolTip(2,"Aansluiting 4 Gebruik de B/W/w notatie geef B-1 voor een wissel in tegen richting.");

    mDriewegWisselDialoog.SetLengte(0, Lengte12);
    mDriewegWisselDialoog.SetLengteToolTip(0, "De lengte in stand 12. Geef -1 als de lengte van blok 1 correct is.");
    mDriewegWisselDialoog.SetLengte(1, Lengte13);
    mDriewegWisselDialoog.SetLengteToolTip(1,"De lengte in stand 13. Geef -1 als de lengte van blok 1 correct is.");
    mDriewegWisselDialoog.SetLengte(2, Lengte13);
    mDriewegWisselDialoog.SetLengteToolTip(2, "De lengte in stand 14. Geef -1 als de lengte van blok 1 correct is.");
    mDriewegWisselDialoog.SetMaxSnelheid(0, MaxSnelheid12);
    mDriewegWisselDialoog.SetMaxSnelheidToolTip(0, "Geef de snelheid limitatie voor 12 stand. Geef -1 voor geen limiet.");
    mDriewegWisselDialoog.SetMaxSnelheid(1, MaxSnelheid13);
    mDriewegWisselDialoog.SetMaxSnelheidToolTip(1, "Geef de snelheid limitatie voor 13 stand. Geef -1 voor geen limiet.");
    mDriewegWisselDialoog.SetMaxSnelheid(2, MaxSnelheid14);
    mDriewegWisselDialoog.SetMaxSnelheidToolTip(2, "Geef de snelheid limitatie voor 14 stand. Geef -1 voor geen limiet.");
}

void DriewegWissel::DialoogOk ()
{
    BlokPointer_t *wisselBlok2;
    BlokPointer_t *wisselBlok4;
    char blokType[2];
    float adres;
    int richting;

    Lengte12 = mDriewegWisselDialoog.GetLengte(0);
    Lengte13 = mDriewegWisselDialoog.GetLengte(1);
    Lengte14 = mDriewegWisselDialoog.GetLengte(2);
    MaxSnelheid12 = mDriewegWisselDialoog.GetMaxSnelheid(0);
    MaxSnelheid13 = mDriewegWisselDialoog.GetMaxSnelheid(1);
    MaxSnelheid14 = mDriewegWisselDialoog.GetMaxSnelheid(2);

    wisselBlok2 = wisselBlok4 = NULL;

    if (sscanf
            (mDriewegWisselDialoog.GetAansluiting(0).c_str(), "%1s%f", blokType, &adres) == 2)
    {
        wisselBlok2 = mWissels.wisselKrijgPointer (blokType[0], adres);
    }
    else
    {
        mMessage.message(str(boost::format
                             ("Blok %s op aansluiting 2 voldoet niet aan B/W/w notatie.")%
                             mDriewegWisselDialoog.GetAansluiting(0).c_str()));
    }
    if (sscanf
            (mDriewegWisselDialoog.GetAansluiting(2).c_str(), "%1s%f", blokType, &adres) == 2)
    {
        wisselBlok4 = mWissels.wisselKrijgPointer (blokType[0], adres);
    }
    else
    {
        mMessage.message(str(boost::format
                             ("Blok %s op aansluiting 4 voldoet niet aan B/W/w notatie.")%
                             mDriewegWisselDialoog.GetAansluiting(2).c_str()));
    }

    if (wisselBlok2 == NULL)
    {
        mMessage.message(str(boost::format
                             ("Het volgend blok %s aanluiting 2 bestaat niet %s") %
                             mDriewegWisselDialoog.GetAansluiting(0).c_str()));
        // wijziging ongedaan gemaakt
    }
    if (wisselBlok4 == NULL)
    {
        mMessage.message(str(boost::format
                             ("Het volgend blok %s aanluiting 4 bestaat niet %s") %
                             mDriewegWisselDialoog.GetAansluiting(2).c_str()));
        // wijziging ongedaan gemaakt
    }

    if (wisselBlok2 == pBlok1)
    {
        mMessage.message(str(boost::format
                             ("Het volgend blok %s aansluiting 2 is hetzelfde als het wissel blok wijzing niet gedaan") %
                             mDriewegWisselDialoog.GetAansluiting(0).c_str()));
        wisselBlok2 = NULL;
    }
    if (wisselBlok4 == pBlok1)
    {
        mMessage.message(str(boost::format
                             ("Het volgend blok %s aansluiting 4 is hetzelfde als het wissel blok wijzing niet gedaan")%
                             mDriewegWisselDialoog.GetAansluiting(2).c_str()));
        wisselBlok4 = NULL;
    }



    if ((wisselBlok2 != NULL) && (wisselBlok4 != NULL))
    {
        // wisselBlok wijst naar een blok
        if ((wisselBlok2 == &mBaanInfo->EindBlokPointer) &&
                (wisselBlok4 == &mBaanInfo->EindBlokPointer))
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

            StopBlokPointer[0].blokRicht[richting] = wisselBlok2;
            mBlok.BlokEndPointInsert (&(StopBlokPointer[0]), richting);

            mBlok.BlokEndPointDelete (&(StopBlokPointer[2]),
                    Richting);

            StopBlokPointer[2].blokRicht[richting] = wisselBlok4;
            mBlok.BlokEndPointInsert (&(StopBlokPointer[2]), richting);

            Richting = richting;
        }
    }
}
