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
#include "ontkoppelaar.h"
#include <time.h>
#include <stdio.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Ontkoppelaar::Ontkoppelaar(IMessage &msg, IBlok& blok, IWissels &wissels, IWisselDialoog &ontKoppelaarDialoog, BaanInfo_t *baanInfo):
    mMessage(msg),
    mBlok(blok),
    mWissels(wissels),
    mOntKoppelaarDialoog(ontKoppelaarDialoog),
    mBaanInfo(baanInfo)
{
    InactiefX1 = 10;
    InactiefX2 = 30;
    InactiefY1 = 20;
    InactiefY2 = 20;
    ActiefX1 = 10;
    ActiefX2 = 30;
    ActiefY1 = 10;
    ActiefY2 = 10;
}

BlokPointer_t * Ontkoppelaar::GetWisselBlok()
{
    return NULL;
}

int Ontkoppelaar::GetRichting()
{
    return 0;
}

int Ontkoppelaar::WisselLengte(int , int )
{
    return 0;
}

void Ontkoppelaar::VeranderBlok (BlokPointer_t * , BlokPointer_t * )
{
}

int Ontkoppelaar::CheckWissel()
{
    return 0;
}

float Ontkoppelaar::GetAdres()
{
    return mOntKoppelaarDialoog.GetAdres();
}

void Ontkoppelaar::InitRoutering ()
{

}

void Ontkoppelaar::ZoekOngeinitializeerdeEindBlokken()
{

}

void Ontkoppelaar::NieuwXY (int , int , int , int)
{

}

int Ontkoppelaar::Init (const char *Input, std::function<std::string()> )
{
  float floatAdres;

  if (sscanf (Input, "%d%f%d%d%d%d%d%d%d%d%d", &Type, &floatAdres,     /* skip spoel nummer */
              &Tijd,
              &InactiefX1,
              &InactiefY1,
              &InactiefX2,
              &InactiefY2,
              &ActiefX1,
              &ActiefY1,
              &ActiefX2,
              &ActiefY2) != 11)
    {
      return 1;
    }

  hardwareAdres = (int) (floatAdres + 0.5);
  hardwareBit =
    (int) ((floatAdres - (float) hardwareAdres) * 100.0 + 0.5);

  Stand = 0;           /* default Inactief */
  return 0;

}



void Ontkoppelaar::Display ()
{


  if (Stand == 0)
    {
//            mBaanInfo->pDC->MoveTo(InactiefX1, InactiefY1);
//            mBaanInfo->pDC->LineTo(InactiefX2, InactiefY2);
    }
  else
    {
//            mBaanInfo->pDC->MoveTo(ActiefX1, ActiefY1);
//            mBaanInfo->pDC->LineTo(ActiefX2, ActiefY2);
    }


}

int Ontkoppelaar::Aanvraag (int)
{
  hardwareArray_t bedien;

  if (Stand == 0)
    {
      Stand = 1;
      Display ();
    }

  bedien.blokIO = HW_IO;
  bedien.adres = hardwareAdres;
  bedien.data = hardwareBit | IO_COMMAND3;
  bedien.nummer = 0;
  bedien.returnGewenst = 0;

  if (mBaanInfo->hardwareHoog.nieuwItem (bedien))
    {
      mMessage.message ("hardware hoog vol info lost ontkoppelaar!");
    }

  TijdTeller = Tijd;  /* Initalizeer de bekrachtig tijd */
  return 0; // altijd goed geen wijgering
}


/* Test of er een verandering is aan de wisselspoel
** De routine geeft een 0 terug als er geen verandering is en een
** 1 als er een verandering plaats heeft gevonden.
**/
void Ontkoppelaar::TestIOTimer()
{
  hardwareArray_t bedien;

  if (Stand == 1)
    {

      if (TijdTeller > 0)
        {
          TijdTeller -= 1;
        }
      else
        {
          if (TijdTeller == 0)
            {
              bedien.blokIO = HW_IO;
              bedien.adres = hardwareAdres;
              bedien.data = hardwareBit | IO_COMMAND0;
              bedien.nummer = 0;
              bedien.returnGewenst = 0;

              if (mBaanInfo->hardwareHoog.nieuwItem (bedien))
                {
                  mMessage.message ("hardware hoog vol info lost ontkoppelaar!");
                }
              Stand = 0;
              Display ();
            }
          else
            {
              /* Spoel oneindig bekrachtiged */
            }
        }
    }
}

std::string Ontkoppelaar::String ()
{
    char string[200];

    sprintf (string, "%d %7.2f %4d %4d %4d %4d %4d %4d %4d %4d %4d",
           Type,
           WisselAdres(),
           Tijd,
           InactiefX1,
           InactiefY1,
           InactiefX2,
           InactiefY2,
           ActiefX1,
           ActiefY1,
           ActiefX2, ActiefY2);

    return string;

}


void Ontkoppelaar::InitDialoog ()
{
  mOntKoppelaarDialoog.SetAdres(WisselAdres());
  mOntKoppelaarDialoog.SetAanTijd(Tijd);
  mOntKoppelaarDialoog.SetAanTijdToolTip("De tijd dat de uitgang actief is in [50ms].");
}

void Ontkoppelaar::DialoogOk ()
{

  Tijd = mOntKoppelaarDialoog.GetAanTijd();
}
