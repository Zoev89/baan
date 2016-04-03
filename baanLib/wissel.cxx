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
#include "ontkoppelaar.h"
#include "lamp.h"
#include "kruising.h"
#include "engelseWissel.h"
#include "driewegWissel.h"
#include "baanSemaphore.h"
#include "draaiSchijf.h"
#include "blok.h"
#include <time.h>
#include <stdio.h>
#include <boost/format.hpp>
#include <memory>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Wissels::Wissels(IMessage &msg, IBlok& blok, IMainWindowDrawing &mainWindowDrawing, IBaanMessage& baanMessage, IWisselDialoog &wisselDialoog, BaanInfo_t *baanInfo,
                 IThreadSleep &threadSleep, IBaanWT& baanWT)
    : mMessage(msg)
    , mBlok(blok)
    , mMainWindowDrawing(mainWindowDrawing)
    , mBaanMessage(baanMessage)
    , mWisselDialoog(wisselDialoog)
    , mBaanInfo(baanInfo)
    , mThreadSleep(threadSleep)
    , mBaanWT(baanWT)
{

}



// Deze routine wordt tijdens de start fase aangeroepen.
// De noodzakelijke structuur velden worden een waarde gegeven
// Return waardes
// WISSEL_ERR_INVALID_ADRES adres of invalid blok/ongebruikt blok
// WISSEL_ERR_NIET_ALLES_AANWEZIG de scanf vond niet alles
// WISSEL_ERR_INVALID_TYPE het type is onbekend
int Wissels::Init (int WisselNummer, const char *Input, std::function<std::string()> extraInput)
{
    int Type, ret_val;


    if (sscanf (Input, "%d", &Type) != 1)
        return 1;

    switch (Type)
    {
    case ONTKOPPELAAR:
    case STANDAARD_WISSEL:
    case KRUISING:
    case ENGELSE_WISSEL:
    case LAMP:
    case DRIEWEG_WISSEL:
    case DRAAISCHIJF:
        break;
    default:
        return WISSEL_ERR_INVALID_TYPE;
    }

    IOBits_t *pWissel;
    pWissel =  mBaanInfo->IOBits[WisselNummer].get();

    pWissel->TijdTeller = 0;
    ret_val =  pWissel->Init(Input, extraInput);
    return ret_val;
}

void Wissels::InitRoutering (int WisselNummer)
{
    mBaanInfo->IOBits[WisselNummer].get()->InitRoutering();
}

void Wissels::ZoekOngeinitializeerdeEindBlokken(int WisselNummer)
{
    mBaanInfo->IOBits[WisselNummer].get()->ZoekOngeinitializeerdeEindBlokken();
}



int Wissels::CheckWissel (int WisselNummer)
{
    return mBaanInfo->IOBits[WisselNummer].get()->CheckWissel();
}

/* Tekenend de wissel op het scherm */
void Wissels::Display (int WisselNummer, bool bitmapRedraw)
{
    IOBits_t *pWissel;
    Rectangle rec;
    pWissel = mBaanInfo->IOBits[WisselNummer].get();


    rec = pWissel->rec;
    if (rec.w () < 0)
    {
        rec.w (-rec.w ());
        rec.h (-rec.h ());
    }
    if (bitmapRedraw)
    {
        mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());
    }

    mBaanInfo->IOBits[WisselNummer].get()->Display ();

    if (mBaanInfo->editMode)
    {
        char string[10];
        sprintf (string, "%d.%02d", pWissel->hardwareAdres,
                 pWissel->hardwareBit);
        // font 12 color blue
        mMainWindowDrawing.DisplayString(pWissel->rec.x () + 1, pWissel->rec.y (), string);
    }
}

/* Als de gebruiker een wissel wil omzetten dan word deze routine
** aangeroepen.
**/
int Wissels::Aanvraag (IOAanvraag_t * aanvraag)
{
    int WisselNummer;
    int ret = 0;                  //  bedient

    // bescherm van interruptie van de work thread
    baanSemaphoreDown (mBaanInfo->semCriticalSection);
    WisselNummer = aanvraag->IONummer;

    ret = mBaanInfo->IOBits[WisselNummer].get()->
            Aanvraag (aanvraag->stand);

    baanSemaphoreUp (mBaanInfo->semCriticalSection);
    return ret;
}



/* Als de gebruiker een wissel wil omzetten dan word deze routine
** aangeroepen.
**/
int Wissels::WisselStand (int *Stand, int WisselNummer)
{
    int Type, return_val = 0;
    auto pWissel = mBaanInfo->IOBits[WisselNummer].get();

    Type = pWissel->Type;
    *Stand = 0;

    switch (Type)
    {
    case ONTKOPPELAAR:
        break;
    case STANDAARD_WISSEL:
        *Stand = pWissel->Stand;
        break;
    case KRUISING:
        break;
    case ENGELSE_WISSEL:
        *Stand = pWissel->Stand;
        break;
    case LAMP:
        break;
    case DRIEWEG_WISSEL:
        *Stand = pWissel->Stand;
        break;
    case DRAAISCHIJF:
        *Stand = pWissel->Stand;
        break;
    default:
        return_val = 1;
        break;
    }

    return return_val;

}



// Doorloop de timers die voor lampen en ontkoppelaars gebruikt worden
void Wissels::TestIOTimers ()
{
    int i;

    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        /* Oke er zijn meer spoelen dan wissels maar zo weet
         ** zeker dat ik alles heb
         **/
        mBaanInfo->IOBits[i].get()->
                TestIOTimer ();
    }
}

// Deze routine zoekt in de IOBits array naar die wissel met
// hetzelfde adres
// Als het adres niet gevonden is wordt -1 returned
int Wissels::ZoekWisselNummer (std::vector<std::unique_ptr<IOBits_t>> &wissels, float adres)
{
    size_t i;
    int hardwareAdres;
    int hardwareBit;

    hardwareAdres = (int) (adres + 0.5);
    hardwareBit = (int) ((adres - (float) hardwareAdres) * 100.0 + 0.5);

    for (i = 0; i < wissels.size(); i++)
    {
        if ((wissels[i].get()->hardwareAdres == hardwareAdres) &&
                (wissels[i].get()->hardwareBit == hardwareBit))
        {
            return i;
        }
    }
    // niet gevonden!
    return -1;
}


// Converteer een wissel adres naar een BlokPointer
// deze routine is gelijk voor alle wissels
// De routine geeft ook info terug of de eventuele
// adres al geinitializeerd is
BlokPointer_t * Wissels::wisselKrijgPointer (int BlokType, float adres)
{
    BlokPointer_t *pBlok;
    int index, blok;

    index = ZoekWisselNummer (mBaanInfo->IOBits, adres);
    blok = (int) adres;

    switch (BlokType)
    {
    case 'B':
    case 'b':
        if (blok == -1)
        {
            pBlok = &mBaanInfo->EindBlokPointer;
        }
        else
        {
            if ((blok < HARDWARE_MIN_ADRES) || (blok >= MAX_NOBLOKS))
                return NULL;
            pBlok = &mBaanInfo->BlokPointer[blok];
            if (pBlok->BlokIONummer == -1)
                return NULL;
        }
        break;
    case 'W':
        if (index == -1)
            return NULL;
        pBlok = &mBaanInfo->IOBits[index].get()->StopBlokPointer[0];
        break;
    case 'w':
        /* Aansluiting op 4 van een drieweg wissel */
        if (index == -1)
            return NULL;
        pBlok = &mBaanInfo->IOBits[index].get()->StopBlokPointer[2];
        break;
    default:
        return NULL;
    }
    return pBlok;
}

std::string Wissels::String (int WisselNummer)
{
    return mBaanInfo->IOBits[WisselNummer].get()->
            String ();
}

void Wissels::NieuwXY (int WisselNummer, int selectionX,
               int selectionY, int deltaX, int deltaY)
{
    IOBits_t *pWissel;
    Rectangle rec;

    pWissel = mBaanInfo->IOBits[WisselNummer].get();

    rec = pWissel->rec;
    if (rec.w () < 0)
    {
        rec.w (-rec.w ());
        rec.h (-rec.h ());
    }
    // maak de box groter want in editMode zetten we ook text neer in het display verhaal
    rec.w (rec.w () + 20);
    rec.y (rec.y () - 3);
    rec.h (rec.h () + 3);
    mMainWindowDrawing.RedrawRec(rec.x(), rec.y(), rec.w(), rec.h());


    mBaanInfo->IOBits[WisselNummer].get()->
            NieuwXY (selectionX, selectionY,
                                    deltaX, deltaY);

}

void Wissels::WisselVeranderBlok (int WisselNummer,
                    BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok)
{
    mBaanInfo->IOBits[WisselNummer].get()->VeranderBlok(oudBlok,nieuwBlok);
}


bool Wissels::InitDialoog (int WisselNummer)
{
    mWisselDialoog.ClearAllFields();
    mBaanInfo->IOBits[WisselNummer].get()->
            InitDialoog ();

    return mWisselDialoog.RunDialogOk();
}

void Wissels::DialoogOk (int WisselNummer)
{
    IOBits_t *pWissel;
    int hardwareAdres;
    int hardwareBit;

    pWissel = mBaanInfo->IOBits[WisselNummer].get();

    float adres = pWissel->GetAdres();

    hardwareAdres = (int) (adres + 0.5);
    hardwareBit = (int) ((adres - (float) hardwareAdres) * 100.0 + 0.5);
    if ((hardwareAdres != pWissel->hardwareAdres)
            || (hardwareBit != pWissel->hardwareBit))
    {
        int i;

        i = ZoekWisselNummer (mBaanInfo->IOBits, adres);

        if (-1 == i)
        {
            // nog niet bestaande spoel
            pWissel->hardwareAdres = hardwareAdres;
            pWissel->hardwareBit = hardwareBit;
        }
        else
        {
            mMessage.message (str(boost::format
                    ("Bestaand adres %.2f dus niet overgenomen") % adres));
        }

    }

    mBaanInfo->IOBits[WisselNummer].get()->
            DialoogOk ();
}

void Wissels::CreateNewIO(int type)
{
    CreateNewIO(type, 0);
}

void Wissels::CreateNewIO(int type, int kopBlok)
{
    switch (type)
    {
    case ONTKOPPELAAR:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<Ontkoppelaar>(mMessage, mBlok, *this, mWisselDialoog, mBaanInfo)));
        break;
    case LAMP:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<Lamp>(mMessage, mBlok, *this, mBaanMessage, mMainWindowDrawing, mWisselDialoog, mBaanInfo)));
        break;
    case STANDAARD_WISSEL:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<StandaardWissel>(mMessage, mBlok, *this, mBaanMessage, mMainWindowDrawing, mWisselDialoog, mBaanInfo, kopBlok)));
        break;
    case KRUISING:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<Kruising>(mMessage, mBlok, *this, mBaanMessage, mMainWindowDrawing, mWisselDialoog, mBaanInfo, kopBlok)));
        break;
    case ENGELSE_WISSEL:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<EngelseWissel>(mMessage, mBlok, *this, mBaanMessage, mMainWindowDrawing, mWisselDialoog, mBaanInfo, kopBlok)));
        break;
    case DRIEWEG_WISSEL:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<DriewegWissel>(mMessage, mBlok, *this, mBaanMessage, mMainWindowDrawing, mWisselDialoog, mBaanInfo, kopBlok)));
        break;
    case DRAAISCHIJF:
        mBaanInfo->IOBits.emplace_back(std::move(make_unique<DraaiSchijf>(mMessage, mBlok, *this, mBaanMessage, mMainWindowDrawing, mWisselDialoog, mBaanInfo, mThreadSleep, mBaanWT, kopBlok)));
        break;
    default:
        return;
        break;
    }
}

void Wissels::WisselNieuw (float adres, int type, int kopBlok)
{
    int hardwareAdres, hardwareBit;

    hardwareAdres = (int) (adres + 0.5);
    hardwareBit = (int) ((adres - (float) hardwareAdres) * 100.0 + 0.5);
    if ((hardwareAdres < HARDWARE_MIN_ADRES) ||
            (hardwareAdres > HARDWARE_MAX_ADRES) ||
            (hardwareBit < HARDWARE_IO_LSB_BITNUMMER) ||
            (hardwareBit > HARDWARE_IO_MSB_BITNUMMER))
    {
        mMessage.message (str(boost::format("Ongeldig IO adres %.2f")% adres));
        return;
    }


    if (ZoekWisselNummer (mBaanInfo->IOBits, adres) == -1)
    {
        // wissel niet gevonden dus het is een nieuwe
        switch (type)
        {
        case ONTKOPPELAAR:
        case LAMP:
        case DRAAISCHIJF:
            // hebben geen kopblok
            break;
        case STANDAARD_WISSEL:
        case KRUISING:
        case ENGELSE_WISSEL:
        case DRIEWEG_WISSEL:
            if ((kopBlok <= 0) || (kopBlok >= MAX_NOBLOKS)
                    || (mBaanInfo->BlokPointer[kopBlok].BlokIONummer == -1))
            {
                mMessage.message (str(boost::format("blok %d is ongebruikt") % kopBlok));
                return;
            }
            break;
        default:
            return;
            break;
        }
        CreateNewIO(type, kopBlok);

        IOBits_t *pWissel;
        pWissel = mBaanInfo->IOBits[mBaanInfo->AantalSpoelen].get();
        mBaanInfo->AantalSpoelen += 1;

        pWissel->hardwareAdres = hardwareAdres;
        pWissel->hardwareBit = hardwareBit;
        pWissel->Type = type;
        pWissel->rec.x (10);
        pWissel->rec.y (10);
        pWissel->rec.w (20);
        pWissel->rec.h (15);

    }
    else
    {
        mMessage.message (str(boost::format("Adres al bekend %.2f") % adres));
    }
}

// Zijeffect mBaanInfo->AantalSpoelen -= 1;
void Wissels::WisselVerwijder (int WisselNummer)
{
    // Nu zit er een gat in de IOBits array dit moeten we weer recht brijen.
    // De array is dusdanig geinitializeerd dat er nooit een gat in mag zitten
    // Dus vul het gat weer op door alle bovenliggende wissels 1 naar voren
    // te plaatsen

    // Eerst verplaatsen dan de pointers modificeren
    for (int i = WisselNummer + 1; i < mBaanInfo->AantalSpoelen; i++)
    {
        mBaanInfo->IOBits[i - 1] = std::move(mBaanInfo->IOBits[i]);
        if (mBaanInfo->IOBits[i - 1].get()->StopBlokPointer[0].BlokIONummer == i)
        {
            // kruising en engelse wissel initializeren dit met hun IOBits nummer
            mBaanInfo->IOBits[i - 1].get()->StopBlokPointer[0].BlokIONummer = i - 1;
        }
    }
    mBaanInfo->IOBits.pop_back(); // remove the last element
    mBaanInfo->AantalSpoelen -= 1;
#ifdef NIET_MEER_NODIG
    int i, w, r;
    int offset;
    BlokPointer_t *start, *einde;
    BlokPointer_t *p;

    start = &(mBaanInfo->IOBits[WisselNummer].get()->StopBlokPointer[0]);
    einde =
        mBaanInfo->IOBits[mBaanInfo->AantalSpoelen - 1].get()->StopBlokPointer.back();
    offset =
        (char *) &(mBaanInfo->IOBits[1].get()->StopBlokPointer[0]) -
        (char *) &(mBaanInfo->IOBits[0].get()->StopBlokPointer[0]);

    // trek de offset af bij alle pointers die in de range liggen voor alle wissels
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        for (w = 0; w < mBaanInfo->IOBits[i].get()->StopBlokPointer.size(); w++)
        {
            for (r = 0; r < 2; r++)
            {
                p = mBaanInfo->IOBits[i].get()->StopBlokPointer[w].blokRicht[r];
                if ((p >= start) && (p <= einde))
                {
                    mBaanInfo->IOBits[i].get()->StopBlokPointer[w].blokRicht[r] =
                        (BlokPointer_t
                         *) ((char *) (mBaanInfo->IOBits[i].get()->
                                       StopBlokPointer[w].blokRicht[r]) - offset);
                }
            }
        }
    }

    // Doen nu hetzelfde voor alle baan blokken
    for (i = 0; i < mBaanInfo->AantalBlokken; i++)
    {
        for (r = 0; r < 2; r++)
        {
            p = mBaanInfo->BlokPointer[i].blokRicht[r];
            if ((p >= start) && (p <= einde))
            {
                mBaanInfo->BlokPointer[i].blokRicht[r] = (BlokPointer_t *) ((char
                                                        *)
                                                        (mBaanInfo->BlokPointer
                                                                [i].blokRicht
                                                                [r])
                                                        -
                                                        offset);
            }
        }
    }
#endif
}



// PASOP zijeffect is dat mBaanInfo->aantalSpoelen verlaagt wordt
void Wissels::WisselDelete (int WisselNummer)
{
    WisselVerwijder (WisselNummer);
}

int Wissels::IsBlokEenWisselNummer(BlokPointer_t * pBlok,IOBits_t * eigenWissel)
{
    int retVal;

    retVal = -1;
    for (int i=0; i<mBaanInfo->AantalSpoelen; i++)
    {
        IOBits_t *pWissel = mBaanInfo->IOBits[i].get();
        if (pWissel == eigenWissel)
            continue;
        switch (pWissel->Type)
        {
        case KRUISING:
        case ENGELSE_WISSEL:
            if ((pWissel->GetWisselBlok() == pBlok) ||
                    (&pWissel->StopBlokPointer[0] == pBlok))
            {
                return i;
            }
            break;
        case STANDAARD_WISSEL:
        case DRIEWEG_WISSEL:
            if (pWissel->GetWisselBlok() == pBlok)
            {
                if (pWissel->GetRichting())
                {
                    return i;
                }
                else
                {
                    // doorzoeken of er misschien een in tegenoverrgestelde rchting ligt
                    retVal = i;
                }
            }
            else if (&pWissel->StopBlokPointer[0] == pBlok)
            {
                return i;
            }
            else if (&pWissel->StopBlokPointer[2] == pBlok)
            {
                return i;
            }
            break;
        }
    }
    return retVal;
}

bool Wissels::KanBlokWordenToegevoegd(BlokPointer_t * pBlok, IOBits_t * eigenWissel)
{
    int wisselNummer = IsBlokEenWisselNummer(pBlok, eigenWissel);
    if ( wisselNummer != -1)
    {
        IOBits_t *pWissel;

        pWissel = mBaanInfo->IOBits[wisselNummer].get();
        switch (pWissel->Type)
        {
        case STANDAARD_WISSEL:
        case DRIEWEG_WISSEL:
            if (pWissel->GetRichting() ==0)
                return true;
            break;
        }
        return false;
    }
    return (pBlok->BlokType == BAAN_BLOK);
}

int Wissels::WisselKnoopPuntNummer(int nummer, BlokPointer_t * pBlok)
{
    if (nummer == -1) return -1;
    IOBits_t * pWissel = mBaanInfo->IOBits[nummer].get();
    switch(pWissel->Type)
    {
    case KRUISING:
    case ENGELSE_WISSEL:
        return (pWissel->GetWisselBlok() == pBlok) ? 0 : 3;
        break;
    case STANDAARD_WISSEL:
    case DRIEWEG_WISSEL:
        if (pWissel->GetRichting() == 0)
        {
            return 0;
        }
        else
        {
            if (pWissel->Type == STANDAARD_WISSEL)
            {
                return (pWissel->GetWisselBlok() == pBlok) ? 1 : 2;
            }
            else
            {
                if (pWissel->GetWisselBlok() == pBlok) return 2;
                else if (&pWissel->StopBlokPointer[0] == pBlok) return 1;
                else return 3;
            }
        }
    }
    // eigenlijk kunnen we hier niet komen
    return -1;
}

int Wissels::WisselIndex(IOBits_t * pWissel)
{
    for(size_t i=0; i<mBaanInfo->IOBits.size(); ++i)
    {
        if (pWissel == mBaanInfo->IOBits[i].get())
            return i;
    }
    return 0;
}

void Wissels::WisselCopyBlokListReverse(std::vector<int>& dst, std::vector<int>& src)
{
    for(int i = src.size()-1; i >= 0; i--)
    {
        dst.push_back(src[i]);
    }
}

void Wissels::VolgKnoopPuntReverse(IOBits_t * pWissel, BlokPointer_t * pBlok, int knoopPuntIndex,
                          IOBits_t * eigenWissel)
{
    if (pWissel->routeKnoopPunt[knoopPuntIndex].wisselNummer == -2)
    {
        VolgKnoopPunt(pWissel, pBlok, knoopPuntIndex, eigenWissel, 1);
    }
}

class VoorkeurRichtingBepaling
{
public:
    VoorkeurRichtingBepaling() : 
        m_voorkeurRichting((int)richtingBeiden)
    {
        
    };
    void voegBlokToe(BlokPointer_t * pBlok)
    {
        m_voorkeurRichting &= (int)pBlok->pBlok->richtingVoorkeur;
        if (m_voorkeurRichting == 0)
        {
            printf("WAARSCHUWING met blok %d voorkeurRichting conflict\n",pBlok->BlokIONummer);
            m_voorkeurRichting = (int)richtingBeiden;
        }
    };
    BlokRichtingVoorkeur getVoorKeur()
    {
        return (BlokRichtingVoorkeur)m_voorkeurRichting;
    };
private:
    int m_voorkeurRichting;
};

void Wissels::VolgKnoopPunt(IOBits_t * pWissel, BlokPointer_t * pBlok, int knoopPuntIndex,
                   IOBits_t * eigenWissel, int richting)
{
    VoorkeurRichtingBepaling voorkeurRichting;
    while((pBlok->BlokType == 0) && (IsBlokEenWisselNummer(pBlok, eigenWissel)==-1))
    {
        pWissel->routeKnoopPunt[knoopPuntIndex].blokList.push_back(pBlok->BlokIONummer);
        voorkeurRichting.voegBlokToe(pBlok);
        pBlok = pBlok->blokRicht[richting];
    }
    if (KanBlokWordenToegevoegd(pBlok, eigenWissel))
    {
        pWissel->routeKnoopPunt[knoopPuntIndex].blokList.push_back(pBlok->BlokIONummer);
        voorkeurRichting.voegBlokToe(pBlok);
    }
    int nummer = IsBlokEenWisselNummer(pBlok, eigenWissel);
    int kpn = WisselKnoopPuntNummer(nummer, pBlok);
    pWissel->routeKnoopPunt[knoopPuntIndex].wisselNummer = nummer;
    pWissel->routeKnoopPunt[knoopPuntIndex].knoopPunt = kpn;
    pWissel->routeKnoopPunt[knoopPuntIndex].richtingVoorkeur = voorkeurRichting.getVoorKeur();

    // init de terugweg
    if (nummer>=0)
    {
        mBaanInfo->IOBits[nummer].get()->routeKnoopPunt[kpn].wisselNummer = WisselIndex(pWissel);
        mBaanInfo->IOBits[nummer].get()->routeKnoopPunt[kpn].knoopPunt = knoopPuntIndex;
        mBaanInfo->IOBits[nummer].get()->routeKnoopPunt[kpn].richtingVoorkeur = voorkeurRichting.getVoorKeur();
        WisselCopyBlokListReverse(
            mBaanInfo->IOBits[nummer].get()->routeKnoopPunt[kpn].blokList,
            pWissel->routeKnoopPunt[knoopPuntIndex].blokList);
    }
}

EersteWisselResult Wissels::VindEersteWissel(int blokNummer, int richting)
{
    int gevonden1,gevonden2;
    int offset1,offset2;
    gevonden1=gevonden2=offset1=offset2= -1;
    for(int i=0; i<mBaanInfo->AantalSpoelen; i++)
    {
        IOBits_t * pWissel = mBaanInfo->IOBits[i].get();
        for (size_t aantal=0; aantal<pWissel->routeKnoopPunt.size(); aantal++)
        {
            for(unsigned int v=0; v<pWissel->routeKnoopPunt[aantal].blokList.size(); v++)
            {
                if (pWissel->routeKnoopPunt[aantal].blokList[v]==blokNummer)
                {
                    gevonden2=gevonden1;
                    gevonden1 = i;
                    offset2=offset1;
                    offset1 = aantal;
                }
            }
        }
    }
    if (gevonden1 == -1) return EersteWisselResult(gevonden1,offset1); // zou eigenlijk niet mogen gebeuren
    
    IOBits_t *pWissel = mBaanInfo->IOBits[gevonden1].get();
    switch (pWissel->Type)
    {
    case KRUISING:
    case ENGELSE_WISSEL:
        if (((offset1 == 0) || (offset1 == 3)) && (richting == 0))
            return EersteWisselResult(gevonden1,offset1);
        if (((offset1 == 1) || (offset1 == 2)) && (richting == 1))
            return EersteWisselResult(gevonden1,offset1);
        break;
    case STANDAARD_WISSEL:
    case DRIEWEG_WISSEL:
        if ((offset1 == 0) && (richting == 0) && (pWissel->GetRichting() == 0))
            return EersteWisselResult(gevonden1,offset1);
        if ((offset1 == 0) && (richting == 1) && (pWissel->GetRichting() == 1))
            return EersteWisselResult(gevonden1,offset1);
        if ((offset1 >=1) && (offset1<=3)
                && (richting == 0) && (pWissel->GetRichting() == 1))
            return EersteWisselResult(gevonden1,offset1);
        if ((offset1 >=1) && (offset1<=3)
                && (richting == 1) && (pWissel->GetRichting() == 0))
            return EersteWisselResult(gevonden1,offset1);
        break;
    }
    return EersteWisselResult(gevonden2,offset2);
}

int Wissels::wisselStand(int wisselNummer, int knp1, int knp2)
{
    int stand = 0;
    IOBits_t *pWissel = mBaanInfo->IOBits[wisselNummer].get();
    switch (pWissel->Type)
    {
    case KRUISING:
    case ENGELSE_WISSEL:
    {
        int verschil = abs(knp1-knp2);
        stand = (verschil==1) ? 12:13;
        break;
    }
    case STANDAARD_WISSEL:
    case DRIEWEG_WISSEL:
    {
        int verschil = abs(knp1-knp2);
        stand = 11+verschil;
        break;
    }
    }
    return stand;
}

int Wissels::wisselLengte(int initialKnoopPunt, int wisselNummer, int knoopPuntNummer)
{
    return  mBaanInfo->IOBits[wisselNummer].get()->WisselLengte(initialKnoopPunt, knoopPuntNummer);
}
