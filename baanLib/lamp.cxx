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
#include "lamp.h"
#include <time.h>
#include <stdio.h>
#include "baanMessage.h"
#include <boost/format.hpp>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Lamp::Lamp(IMessage &msg, IBlok& blok, IWissels &wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, IWisselDialoog &lampDialoog, BaanInfo_t *baanInfo):
    IOBits_t(baanInfo->EindBlokPointer),
    mMessage(msg),
    mBlok(blok),
    mWissels(wissels),
    mBaanMessage(baanMessage),
    mMainWindowDrawing(mainWindowDrawing),
    mLampDialoog(lampDialoog),
    mBaanInfo(baanInfo)
{
    hwAan = 1;
    hwTot = 8;
    hwType = 2;
    UitTijd = -1;
    Tijd = -1;
}

BlokPointer_t * Lamp::GetWisselBlok()
{
    return NULL;
}

int Lamp::GetRichting()
{
    return 0;
}

int Lamp::WisselLengte(int , int )
{
    return 0;
}

int Lamp::CheckWissel()
{
    return 0;
}

void Lamp::VeranderBlok (BlokPointer_t * , BlokPointer_t * )
{
}

void Lamp::InitRoutering ()
{

}

void Lamp::ZoekOngeinitializeerdeEindBlokken()
{

}

int Lamp::Init (char *Input, FILE *)
{
    float floatAdres;
    int x, y, w, h;

    /* Lees alle velden in */
    if (sscanf (Input, "%d%f%d%d%d%d%d%d%d%d%d", &Type, &floatAdres, &hwAan,       // niet gebruikt alleen belanrijk voor de microcontroller
                &hwTot,     // niet gebruikt alleen belanrijk voor de microcontroller
                &hwType,    // niet gebruikt alleen belanrijk voor de microcontroller
                &Tijd, &UitTijd, &x, &y, &w, &h) != 11)
    {
        return 1;
    }
    hardwareAdres = (int) (floatAdres + 0.5);
    hardwareBit =
            (int) ((floatAdres - (float) hardwareAdres) * 100.0 + 0.5);
    Stand = 0;           /* default Inactief */
    aan = 0;

    rec.x (x);
    rec.y (y);
    rec.h (h);
    rec.w (w);

    return 0;
}



void Lamp::Display ()
{
    Rectangle rec;

    rec = rec;
    if (rec.w () < 0)
    {
        rec.w (-rec.w ());
        rec.h (-rec.h ());
    }

    if (aan == 1)
    {
        mMainWindowDrawing.VierkantDisplay(rec.x(), rec.y(), rec.w(), rec.h(), IMainWindowDrawing::Yellow);
    }
    else
    {
        mMainWindowDrawing.VierkantDisplay(rec.x(), rec.y(), rec.w(), rec.h(), IMainWindowDrawing::Gray66);
    }

}


int Lamp::Aanvraag (int stand)
{
    hardwareArray_t bedien;

    if (stand == Stand)
    {
        // status gelijk dus we hoeven niets te doen
        // geef aan alsof die bedient is
        return 0;
    }

    if (Stand == 0)
    {
        Stand = 1;
        aan = 1;
        TijdTeller = Tijd;      /* Initalizeer de bekrachtig tijd */
    }
    else
    {
        Stand = 0;
        TijdTeller = 0;
    }

    bedien.blokIO = HW_IO;
    bedien.adres = hardwareAdres;
    bedien.data = hardwareBit;   // default IO_COMMAND0
    bedien.nummer = 0;
    bedien.returnGewenst = 0;

    if (Stand == 1)
    {
        // wissel gebogen
        bedien.data |= IO_COMMAND3;
        // send message alleen wanneer aan
        // het uit gaan wordt door de baanWT via TestSpoelLamp gedaan
        mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);
    }
    if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
    {
        mMessage.message ("hardware hoog vol info lost lamp!");
    }

    return 0;
}

// TODO remove this when mBaanMessage.Post supports a std::function
int Lamp::WisselNummer()
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


/* Test of er een verandering aan de wisselspoel moet plaats vinden.
** Indien nodig word de lamp van status verandered
**/
void Lamp::TestIOTimer ()
{
    hardwareArray_t bedien;

    if (Stand == 1)
    {
        if (TijdTeller > 0)
        {
            TijdTeller -= 1;
        }
        if (TijdTeller == 0)
        {
            if (aan == 1)
            {
                /* Spoel nog set dus reset nu */
                aan = 0;
                TijdTeller = UitTijd;

                bedien.blokIO = HW_IO;
                bedien.adres = hardwareAdres;
                bedien.data = hardwareBit | IO_COMMAND0;
                bedien.nummer = 0;
                bedien.returnGewenst = 0;

                if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
                {
                    mMessage.message ("hardware hoog vol info lost lamp!");
                }
                // Als er een uittijd gespecificeerd is dan houd
                // de lamp dan zoveel cycles uit
                if (TijdTeller <= 0)
                {
                    // Er was geen uittijd gegeven dus na een cycle
                    // moet die uit.
                    Stand = 0;
                }
            }
            else
            {
                // De stand was nog actief dus schakel de lamp weer
                // aan.
                aan = 1;
                bedien.blokIO = HW_IO;
                bedien.adres = hardwareAdres;
                bedien.data = hardwareBit | IO_COMMAND3;
                bedien.nummer = 0;
                bedien.returnGewenst = 0;

                if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
                {
                    mMessage.message ("hardware hoog vol info lost lamp!");
                }
                TijdTeller = Tijd;
                // Ik hoef de TijdTeller niet te testen want dat was
                // de eerste kaar al gedaan.
            }
            mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);

        }
        else
        {
            /* Spoel oneindig bekrachtiged */
        }
    }
    else
    {
        /* Stand is 0 check de spoel of die nog aan is */
        if (aan == 1)
        {
            /* Spoel nog set dus reset nu */
            aan = 0;
            Stand = 0;

            mBaanMessage.Post (WM_WISSEL_DISPLAY, WisselNummer(), 0, 0);
            bedien.blokIO = HW_IO;
            bedien.adres = hardwareAdres;
            bedien.data = hardwareBit | IO_COMMAND0;
            bedien.nummer = 0;
            bedien.returnGewenst = 0;

            if (mBaanInfo->hardwareHoog.nieuwItem (&bedien))
            {
                mMessage.message ("hardware hoog vol info lost lamp!");
            }

        }
    }
}

void Lamp::String (char *string)
{
    sprintf (string, "%d %7.2f %5d %5d %5d %5d %5d %4d %4d %4d %4d", Type, WisselAdres(), hwAan,    // niet gebruikt alleen belanrijk voor de microcontroller
             hwTot, // niet gebruikt alleen belanrijk voor de microcontroller
             hwType,        // niet gebruikt alleen belanrijk voor de microcontroller
             Tijd,
             UitTijd,
             rec.x (),
             rec.y (), rec.w (), rec.h ());
}


void Lamp::NieuwXY (int /*selectionX*/,
                    int /*selectionY*/, int deltaX, int deltaY)
{
    mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());
    rec.move (deltaX, deltaY);
    Display ();
}


void Lamp::InitDialoog ()
{
    mLampDialoog.SetUitleg("voor een aan uit lamp specificeer pwm 0/1 met\n"
                           "hwAan = 1 hwTot = 1\n"
                           "voor een dimbare lamp met 8 stappen specificeer pwm 0/1 met\n"
                           "hwAan = 1 hwTot = 8"
                           );
    mLampDialoog.SetAdres(WisselAdres());
    mLampDialoog.SetHwAan(hwAan);
    mLampDialoog.SetHwAanToolTip("De tijd dat uitgang van de microcontroller actief is in 1/2880 sec.");
    mLampDialoog.SetHwTot(hwTot);
    mLampDialoog.SetHwTotToolTip("De totale cycle tijd dat de microcontroller actief is in 1/2880 sec.");
    mLampDialoog.SetTypeIndex(hwType);
    std::vector<std::string> typeLabels;
    typeLabels.push_back("OneShot 0/1");
    typeLabels.push_back("OneShot p/n");
    typeLabels.push_back("pwm 0/1");
    typeLabels.push_back("freq p/n");
    mLampDialoog.SetTypeLabels(typeLabels);

    mLampDialoog.SetAanTijd(aan);
    mLampDialoog.SetAanTijdToolTip("De tijd dat de uitgang actief is in [50ms].");
    mLampDialoog.SetUitTijd(UitTijd);
    mLampDialoog.SetUitTijdToolTip("De tijd dat de uitgang uit is voordat die weer aangaat [50ms].");
    mLampDialoog.SetHoogte(rec.h());
    mLampDialoog.SetHoogteToolTip("Geef een negatief getal als die niet bedient wordt door het gebruikers interface.");
    mLampDialoog.SetBreedte(rec.w());
    mLampDialoog.SetBreedteToolTip("Geef een negatief getal als die niet bedient wordt door het gebruikers interface.");
}

float Lamp::GetAdres()
{
    return mLampDialoog.GetAdres();
}

void Lamp::DialoogOk ()
{
    hwAan = mLampDialoog.GetHwAan();
    hwTot = mLampDialoog.GetHwTot();
    hwType = mLampDialoog.GetTypeIndex();
    aan = mLampDialoog.GetAanTijd();
    UitTijd = mLampDialoog.GetUitTijd();
    rec.w (mLampDialoog.GetBreedte());
    rec.h (mLampDialoog.GetHoogte());
}

