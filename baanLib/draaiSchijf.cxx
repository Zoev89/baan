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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DraaiSchijf::DraaiSchijf(IMessage &msg, IBlok& blok, IWissels &wissels, IBaanMessage &baanMessage, IMainWindowDrawing &mainWindowDrawing, IWisselDialoog &standaardWisselDialoog, BaanInfo_t *baanInfo, int kopBlok):
    mMessage(msg),
    mBlok(blok),
    mWissels(wissels),
    mBaanMessage(baanMessage),
    mMainWindowDrawing(mainWindowDrawing),
    mStandaardWisselDialoog(standaardWisselDialoog),
    mBaanInfo(baanInfo)
{
    pBlok1 = &mBaanInfo->BlokPointer[kopBlok];

}

DraaiSchijf::~DraaiSchijf()
{


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

void DraaiSchijf::VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok)
{
    if (pBlok1 == oudBlok)
    {
        pBlok1 = nieuwBlok;
    }
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

void DraaiSchijf::TestIOTimer ()
{

}

float DraaiSchijf::GetAdres()
{
    return mStandaardWisselDialoog.GetAdres();
}

void DraaiSchijf::UpdateRec ()
{
    // implementeer iets
}

int DraaiSchijf::Init (const char *Input, std::function<std::string()> extraInput)
{
    float floatAdres;

    /* Lees alle velden in */
    if (sscanf (Input, "%d%f%d%d%d",
                &Type,
                &floatAdres,
                &Coord1X,
                &Coord1Y,
                &Radius
                ) != 5)
        return WISSEL_ERR_NIET_ALLES_AANWEZIG;
    hardwareAdres = (int)floatAdres;
    hardwareBit = 0;
    if ((hardwareAdres < HARDWARE_MIN_ADRES) || (hardwareAdres >= MAX_NOBLOKS))
    {
        return WISSEL_ERR_INVALID_ADRES;
    }
/*    if (mBaanInfo->BlokPointer[Blok1].BlokIONummer == -1)
    {
        return WISSEL_ERR_INVALID_ADRES;
    }*/

    pBlok1 = &mBaanInfo->BlokPointer[hardwareAdres];

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

    if (Stand == 12)
    {
//        mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
//                                    Coord2X, Coord2Y,
//                                    5, color);
    }
    else
    {
//        mMainWindowDrawing.DrawLine(Coord1X, Coord1Y,
//                                    Coord3X, Coord3Y,
//                                    5, color);
    }
}

void DraaiSchijf::Bedien ()
{
    hardwareArray_t bedien;

    bedien.blokIO = HW_IO;
    bedien.adres = hardwareAdres;
    bedien.data = 0; //hardwareBit;   // default IO_COMMAND0
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

int DraaiSchijf::Aanvraag (int stand)
{
    return 0;
}


void DraaiSchijf::String (char *string)
{
    char sString[20];
    char bString[20];


//    if (Richting)
//    {
//        // richting terug
//        strcpy (sString, "B-1");
//    }
//    else
//    {
//        mBlok.BlokNaam (sString, StopBlokPointer[0].pVolgendBlok);
//    }
//    mBlok.BlokNaam (bString, pBlok1);

    sprintf (string, "%d %7.2f %4d %4d %4d",
             Type,
             WisselAdres(),
             Coord1X,
             Coord1Y,
             Radius
             );
}

void DraaiSchijf::NieuwXY (int selectionX,
                               int selectionY, int deltaX, int deltaY)
{
    if (mBaanInfo->selectedWisselPoint == -1)
    {
//        int afstand1, afstand2, x, y;
//        afstand1 = SQR (Coord1X - selectionX) +
//                SQR (Coord1Y - selectionY);
//        if (Stand == 12)
//        {
//            mBaanInfo->selectedWisselPoint = 2;
//            x = Coord2X;
//            y = Coord2Y;
//        }
//        else
//        {
//            mBaanInfo->selectedWisselPoint = 3;
//            x = Coord3X;
//            y = Coord3Y;
//        }
//        afstand2 = SQR (x - selectionX) + SQR (y - selectionY);
//        if (afstand1 < afstand2)
//        {
//            afstand2 = afstand1;
//            mBaanInfo->selectedWisselPoint = 1;
//        }
//        x = rec.center_x ();
//        y = rec.center_y ();
//        afstand1 = SQR (x - selectionX) + SQR (y - selectionY);
//        if (afstand1 < afstand2)
//        {
//            mBaanInfo->selectedWisselPoint = 4;
//        }

    }

//    switch (mBaanInfo->selectedWisselPoint)
//    {
//    case 1:
//        Coord1X += deltaX;
//        Coord1Y += deltaY;
//        break;
//    case 2:
//        Coord2X += deltaX;
//        Coord2Y += deltaY;
//        break;
//    case 3:
//        Coord3X += deltaX;
//        Coord3Y += deltaY;
//        break;
//    case 4:
//        Coord1X += deltaX;
//        Coord1Y += deltaY;
//        Coord2X += deltaX;
//        Coord2Y += deltaY;
//        Coord3X += deltaX;
//        Coord3Y += deltaY;
//        break;
//    }
    mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());
    UpdateRec ();

    Display();
}

void DraaiSchijf::InitDialoog ()
{
    char string[20];

    mStandaardWisselDialoog.SetUitleg(
                "12 is de rechte kant en 13 is buigend.\n"
                "Een gebogen wissel 12 is de grote bocht en 13 is\n"
                "de krappe bocht.\n"
                "Een wissel kan op twee manieren liggen namelijk:\n"
                "  Voorwaards         Achterwaards\n"
                "       /----2--    --3----\n"
                " -1---/                   /---1-\n"
                "        ----3--    --2----/\n"
                "Een voorwaardse wissel 3 is het volgende blok/wissel.\n"
                "Een achterwaardse wissel 3 is B-1 deze wordt dan door\n"
                "een andere wissel of baanvak ingevult (Wxxxx.yy)\n"
                "Bij een negatieve lengte wordt de lengte van het\n"
                "blok genomen\n");
    mStandaardWisselDialoog.SetAdres(WisselAdres());

    mBlok.BlokNaam (string, StopBlokPointer[0].blokRicht[0]);
    /*
    mStandaardWisselDialoog.SetAansluiting3(string);
    mStandaardWisselDialoog.SetAansluiting3ToolTip("Gebruik de B/W/w notatie geef B-1 voor een wissel in tegen richting.");
    mStandaardWisselDialoog.SetLengte12(Lengte12);
    mStandaardWisselDialoog.SetLengte12ToolTip("De lengte in stand 12. Geef -1 als de lengte van blok 1 correct is.");
    mStandaardWisselDialoog.SetLengte13(Lengte13);
    mStandaardWisselDialoog.SetLengte13ToolTip("De lengte in stand 13. Geef -1 als de lengte van blok 1 correct is.");
    mStandaardWisselDialoog.SetMaxSnelheid12(MaxSnelheid12);
    mStandaardWisselDialoog.SetMaxSnelheid12ToolTip("Geef de snelheid limitatie voor 12 stand. Geef -1 voor geen limiet.");
    mStandaardWisselDialoog.SetMaxSnelheid13(MaxSnelheid13);
    mStandaardWisselDialoog.SetMaxSnelheid13ToolTip("Geef de snelheid limitatie voor 13 stand. Geef -1 voor geen limiet.");
    */
}

void DraaiSchijf::DialoogOk ()
{
    BlokPointer_t *wisselBlok;
    char blokType[2];
    float adres;
    int richting;

/*
    Lengte12 = mStandaardWisselDialoog.GetLengte12();
    Lengte13 = mStandaardWisselDialoog.GetLengte13();
    MaxSnelheid12 = mStandaardWisselDialoog.GetMaxSnelheid12();
    MaxSnelheid13 = mStandaardWisselDialoog.GetMaxSnelheid13();


    // volgend blok gewijzigd
    if (sscanf
            (mStandaardWisselDialoog.GetAansluiting3().c_str(), "%1s%f", blokType, &adres) == 2)
    {
        wisselBlok = mWissels.wisselKrijgPointer (blokType[0], adres);
        if (wisselBlok == NULL)
        {
            mMessage.message(str(boost::format
                                 ("Het volgend blok bestaat niet %s") %
                                 mStandaardWisselDialoog.GetAansluiting3().c_str()));
            // wijziging ongedaan gemaakt
        }
        else if (wisselBlok == pBlok1)
        {
            mMessage.message(str(boost::format
                                 ("Het volgend blok %s is hetzelfde als het wissel blok wijzing niet gedaan")%
                                 mStandaardWisselDialoog.GetAansluiting3().c_str()));
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
                             mStandaardWisselDialoog.GetAansluiting3().c_str()));
    }
    */
}
