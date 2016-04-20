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
#include "kruising.h"
#include "leesdata.h"
#include "blok.h"
#include <time.h>
#include <stdio.h>
#include <boost/format.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Kruising::Kruising(IMessage &msg, IBlok& blok, IWissels &wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, IWisselDialoog &kruisingDialoog, BaanInfo_t *baanInfo, int kopBlok):
    mMessage(msg),
    mBlok(blok),
    mWissels(wissels),
    mBaanMessage(baanMessage),
    mMainWindowDrawing(mainWindowDrawing),
    mKruisingDialoog(kruisingDialoog),
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
    MaxSnelheid13 = -1;
    MaxSnelheid14 = -1;
    Lengte13 = -1;
    Lengte42 = -1;
    StopBlok.resize(1);
    StopBlokPointer.resize(1);
    routeKnoopPunt.resize(4);
    IOBits_t::InitConstructor(baanInfo->EindBlokPointer);
    StopBlokPointer[0].pBlok =
        pBlok1->pBlok;
}


Kruising::~Kruising()
{
    // StopBlokPointer wordt aan beiden zijde gebruikt
    //mBlok.BlokEndPointDelete (&StopBlokPointer[0], 0);
    //mBlok.BlokEndPointDelete (&StopBlokPointer[0], 1);
}


BlokPointer_t * Kruising::GetWisselBlok()
{
    return pBlok1;
}

int Kruising::GetRichting()
{
    return 0;
}

int Kruising::WisselLengte(int from, int to)
{
    int lengte = - pBlok1->Lengte;
    switch(from*10 + to)
    {
    case 02:
    case 20:
        lengte += abs(Lengte13);
        break;
    case 31:
    case 13:
        lengte += abs(Lengte42);
        break;
    }
    return lengte;
}

void Kruising::VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok)
{
    if (pBlok1 == oudBlok)
    {
        pBlok1 = nieuwBlok;
        StopBlokPointer[0].pBlok = pBlok1->pBlok;
        StopBlokPointer[0].BlokIONummer = WisselNummer();
    }
}

// TODO remove this when mBaanMessage.Post supports a std::function
int Kruising::WisselNummer()
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


int Kruising::CheckWissel()
{

    if (CheckWisselBlok (pBlok1, 0, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
          ("Kruising %d.%02d aansluiting 3 niet goed aangesloten!") %
          hardwareAdres% hardwareBit));
        return 1;
    }
    if (CheckWisselBlok (&StopBlokPointer[0], 0, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
          ("Kruising %d.%02d aansluiting 2 niet goed aangesloten!") %
          hardwareAdres% hardwareBit));
        return 1;
    }
    if (CheckWisselBlok (&StopBlokPointer[0], 1, &mBaanInfo->EindBlokPointer))
    {
        mMessage.message(str(boost::format
          ("Kruising %d.%02d aansluiting 4 niet goed aangesloten!") %
          hardwareAdres% hardwareBit));
        return 1;
    }
    return 0;
}

void Kruising::TestIOTimer ()
{

}

float Kruising::GetAdres()
{
    return mKruisingDialoog.GetAdres();
}

int Kruising::Aanvraag (int)
{
    return 0;
}

void Kruising::UpdateRec ()
{
    int i;

    i = (Coord1X);
    if (Coord2X < i)
        i = Coord2X;
    if (Coord3X < i)
        i = Coord3X;
    if (Coord4X < i)
        i = Coord4X;
    rec.x (i - 2);
    i = (Coord1Y);
    if (Coord2Y < i)
        i = Coord2Y;
    if (Coord3Y < i)
        i = Coord3Y;
    if (Coord4Y < i)
        i = Coord4Y;
    rec.y (i - 2);
    i = (Coord1X);
    if (Coord2X > i)
        i = Coord2X;
    if (Coord3X > i)
        i = Coord3X;
    if (Coord4X > i)
        i = Coord4X;
    rec.w (i - rec.x () + 2);
    i = (Coord1Y);
    if (Coord2Y > i)
        i = Coord2Y;
    if (Coord3Y > i)
        i = Coord3Y;
    if (Coord4Y > i)
        i = Coord4Y;
    rec.h (i - rec.y () + 2);
}


int Kruising::Init (const char *Input, std::function<std::string()> )
{
    float floatAdres;
    int Blok1;
    char Blok2Type[2];
    float adres2;
    BlokPointer_t *pBlok2;

    /* Lees alle velden in */

    // NOOIT GETEST DUS KUNNNEN FOUTEN HEBBEN
    if (sscanf (Input, "%d%f%d%d%d%d%d%d%d%d%d%1s%f",
                &Type,
                &floatAdres,
                &Coord1X,
                &Coord1Y,
                &Coord2X,
                &Coord2Y,
                &Coord3X,
                &Coord3Y,
                &Coord4X,
                &Coord4Y, &Blok1, Blok2Type, &adres2) != 13)
        return WISSEL_ERR_NIET_ALLES_AANWEZIG;

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

    Stand = 0;           /* We hebben geen stand */


    /* Nu het spoel blok ook naar de baan laten wijzen */
    StopBlokPointer[0].pBlok = pBlok1->pBlok;
    StopBlokPointer[0].BlokIONummer = WisselNummer();


    if (&mBaanInfo->EindBlokPointer != StopBlokPointer[0].pVolgendBlok)
    {
        // we hebben een probleem
        mMessage.message(str(boost::format
                             ("Regel %d: Van Wissel %d.%02d aansluiting 24 is al aangesloten met blok %d\n%s\nRichting misschien fout of aansluiting 24 met 13 verwisseld?")%
                             EricFgetsGetLastLineCount ()% hardwareAdres%
                             hardwareBit%
                             StopBlokPointer[0].pVolgendBlok->BlokIONummer% Input));
        return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
    }

    if (&mBaanInfo->EindBlokPointer != pBlok2->pVorigBlok)
    {
        // we hebben een probleem
        mMessage.message(str(boost::format
                             ("Regel %d: Van Wissel %d.%02d aansluiting 2 is het volgend blok zijn TERUG weg al belegd met blok %d\n%s")%
                             EricFgetsGetLastLineCount ()% hardwareAdres%
                             hardwareBit% pBlok2->pVorigBlok->BlokIONummer% Input));
        return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
    }

    if (pBlok1->pVolgendBlok == &mBaanInfo->EindBlokPointer)
    {
        // we hebben een probleem
        mMessage.message(str(boost::format
                             ("Regel %d: Van Wissel %d.%02d aansluiting 3 is nog niet aangesloten dit had in de blok sectie al gedaan moeten zijn\n%s")%
                             EricFgetsGetLastLineCount ()% hardwareAdres%
                             hardwareBit% Input));
        return WISSEL_ERR_MESSAGE_AL_GEGEVEN;
    }




    // Vul de blok2 in
    StopBlokPointer[0].pVolgendBlok = pBlok2;
    pBlok2->pVorigBlok = &StopBlokPointer[0];
    UpdateRec ();

    return 0;
}

void Kruising::InitRoutering ()
{
    // NOOIT GETEST copy engelse wissel zonder bediening
    routeKnoopPunt[0].aansluitingen.push_back(2);
    routeKnoopPunt[1].aansluitingen.push_back(3);
    routeKnoopPunt[2].aansluitingen.push_back(0);
    routeKnoopPunt[3].aansluitingen.push_back(1);
    BlokPointer_t * pBlok = pBlok1;
    // note voor aansluiting 2 -> offset 1 het blok in de lijst want
    // stopblok heeft het wisselnummer
    routeKnoopPunt[1].blokList.push_back(pBlok->BlokIONummer);
    mWissels.VolgKnoopPunt(this, pBlok, 2, this);
    pBlok = &StopBlokPointer[0];
    pBlok = pBlok->pVolgendBlok;
    mWissels.VolgKnoopPunt(this, pBlok, 1, this);
}

void Kruising::ZoekOngeinitializeerdeEindBlokken()
{
    BlokPointer_t * pBlok = pBlok1;
    pBlok = pBlok->pVorigBlok;
    mWissels.VolgKnoopPuntReverse(this, pBlok, 0);
    pBlok = &StopBlokPointer[0];
    pBlok = pBlok->pVorigBlok;
    mWissels.VolgKnoopPuntReverse(this, pBlok, 3);
}

void Kruising::Display()
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

    mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
                                Coord3X, Coord3Y,
                                5, color);
    mMainWindowDrawing.DrawLine(Coord2X, Coord2Y,
                                Coord4X, Coord4Y,
                                5, color);

}

std::string Kruising::String ()
{
    char sString[20];
    char bString[20];
    char string[200];

    mBlok.BlokNaam (sString, StopBlokPointer[0].pVolgendBlok);
    mBlok.BlokNaam (bString, pBlok1);

    sprintf (string, "%d %7.2f %4d %4d %4d %4d %4d %4d %4d %4d %8s %8s",
             Type,
             WisselAdres(),
             Coord1X,
             Coord1Y,
             Coord2X,
             Coord2Y,
             Coord3X,
             Coord3Y,
             Coord4X,
             Coord4Y, &bString[1], sString);
    return string;
}


void Kruising::NieuwXY (int selectionX,
                        int selectionY, int deltaX, int deltaY)
{
    if (mBaanInfo->selectedWisselPoint == -1)
    {
        int afstand1, afstand2, x, y;

        mBaanInfo->selectedWisselPoint = 1;
        afstand1 = SQR (Coord1X - selectionX) +
                SQR (Coord1Y - selectionY);
        x = Coord2X;
        y = Coord2Y;
        afstand2 = SQR (x - selectionX) + SQR (y - selectionY);
        if (afstand2 < afstand1)
        {
            mBaanInfo->selectedWisselPoint = 2;
            afstand1 = afstand2;
        }
        x = Coord3X;
        y = Coord3Y;
        afstand2 = SQR (x - selectionX) + SQR (y - selectionY);
        if (afstand2 < afstand1)
        {
            mBaanInfo->selectedWisselPoint = 3;
            afstand1 = afstand2;
        }
        x = Coord4X;
        y = Coord4Y;
        afstand2 = SQR (x - selectionX) + SQR (y - selectionY);
        if (afstand2 < afstand1)
        {
            mBaanInfo->selectedWisselPoint = 4;
            afstand1 = afstand2;
        }
        x = rec.center_x ();
        y = rec.center_y ();
        afstand2 = SQR (x - selectionX) + SQR (y - selectionY);
        if (afstand2 < afstand1)
        {
            mBaanInfo->selectedWisselPoint = 5;
            afstand1 = afstand2;
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
    case 5:                    // centrum alle punten
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

void Kruising::InitDialoog ()
{
    char string[20];


    mKruisingDialoog.SetUitleg(
                " 1 en 3 aansluiten via de blokken\n"
                " 4 vanuit een blok of een wissel (Wxxxx.yy)\n"
                " 2 via de initializatie aangeven\n"
                "\n"
                " Je kunt wel rechts om of links om tellen\n"
                "            ===>rijrichting\n"
                " -1-              -2-      -4-               -3-\n"
                "    ---        ---             ---        ---\n"
                "       ---  ---                   ---  ---\n"
                "          ==                         ==\n"
                "       ---  ---                   ---  ---\n"
                "    ---        ---             ---        ---\n"
                " -4-              -3-       -1-              -2-\n"
                );
    mKruisingDialoog.SetAdres(WisselAdres());
    mBlok.BlokNaam (string, StopBlokPointer[0].blokRicht[0]);

    mKruisingDialoog.SetAansluiting(0, string);
    mKruisingDialoog.SetAansluitingToolTip(0, "Aansluiting 2 gebruik de B/W/w notatie geef B-1 voor een wissel in tegen richting.");
    mKruisingDialoog.SetLengte(1, Lengte13);
    mKruisingDialoog.SetLengteToolTip(1, "De lengte in stand 13. Geef -1 als de lengte van blok 1 correct is.");
    mKruisingDialoog.SetLengte(2,Lengte42);
    mKruisingDialoog.SetLengteToolTip(2,"De lengte in stand 42. Geef -1 als de lengte van blok 1 correct is.");
    mKruisingDialoog.SetMaxSnelheid(1,MaxSnelheid13);
    mKruisingDialoog.SetMaxSnelheidToolTip(1,"Geef de snelheid limitatie voor 13 stand. Geef -1 voor geen limiet.");
}

void Kruising::DialoogOk ()
{
    BlokPointer_t *wisselBlok;
    char blokType[2];
    float adres;


    Lengte13 = mKruisingDialoog.GetLengte(1);
    Lengte42 = mKruisingDialoog.GetLengte(2);
    MaxSnelheid13 = mKruisingDialoog.GetMaxSnelheid(1);

    // volgend blok gewijzigd
    if (sscanf
            (mKruisingDialoog.GetAansluiting(0).c_str(), "%1s%f", blokType, &adres) == 2)
    {
        wisselBlok = mWissels.wisselKrijgPointer (blokType[0], adres);
        if (wisselBlok == NULL)
        {
            mMessage.message(str(boost::format
                                 ("Het volgend blok bestaat niet %s") %
                                 mKruisingDialoog.GetAansluiting(0).c_str()));
            // wijziging ongedaan gemaakt
        }
        else if (wisselBlok == pBlok1)
        {
            mMessage.message(str(boost::format
                                 ("Het volgend blok %s is hetzelfde als het wissel blok wijzing niet gedaan") %
                                 mKruisingDialoog.GetAansluiting(0).c_str()));
        }
        else
        {
            // wisselBlok wijst naar een blok

            mBlok.BlokEndPointDelete (&(StopBlokPointer[0]), 0);

            StopBlokPointer[0].blokRicht[0] = wisselBlok;
            mBlok.BlokEndPointInsert (&(StopBlokPointer[0]), 0);

        }
    }
    else
    {
        mMessage.message(str(boost::format
                             ("Blok %s op aansluiting 2 voldoet niet aan B/W/w notatie.") %
                             mKruisingDialoog.GetAansluiting(0).c_str()));
    }


}
