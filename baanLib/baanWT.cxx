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
#include "baanMessage.h"
#include "baanSemaphore.h"
#include "wissel.h"
#include "td.h"
#include "baanTime.h"
#include "usbinout.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <assert.h>
#include "IView.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#endif
#include <boost/format.hpp>

#define DEBUG_TIME


#define MAAK_NEGATIEF 0x40

#define HANDREGELAAR_IDLE         0x1f
#define HANDREGELAAR_HARDER       0x1e
#define HANDREGELAAR_LANGZAMER    0x1d
#define HANDREGELAAR_VT           0x1b
#define HANDREGELAAR_STOP         0x17
#define HANDREGELAAR_VOLGENDE     0x0f
#define HANDREGELAAR_IGNORE_STOP  0x15

// minimale spanning voordat ik een detectie zie
#define AANTAL_DETECTIES 1      /* hoeveel detecties we hebben voordat we hem vertrouwen */
#define DETECT_DREMPEL 2

// Defines voor de laatste wagon detectie
#define LAATSTE_WAGON_DECIMATIE 3       // decimatie 3 zodat we niet teveel events genereren
// 2 is nooit genoeg 3 is bijna goed en 4 is betrouwbaar
#define AANTAL_LAATSTE_WAGON_DETECTIES 4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define SLEEP_TIME (CYCLETIME/2)

#define LOOPTIJD_BEREKENING 1000        // om de 1000 cycles 'execution time? TODO check it'

// Tja het blijk dat de Sleep functie wel eens iets te vroeg
// terugkomt. Om dit te voorkomen vandaar deze routine die dat controlleerd
// en eventueel compenseerd
// Er is nu zelfs gepoogd om via de performance counters van de pentium 
// de tijd zo goed mogelijk te benaderen. 
// Input tijd is in ms

void
BaanWT::ericSleep (int tijd)
{

#ifdef _WIN32
    _int64 t;
    _int64 eind;
    t = readTSC ();
#if  _MSC_VER>1100              // 1100 is voor version 5.0 en die bestuurd de hardware
    if (tijd > 100)               // mijn labtop scheduled niet goed in sumilatie
        // TODO verbeter het schedulen van de labtop....
#else
    if (tijd > 10)
#endif
    {
        // TODO compenseer de CPU klok ik heb nu 1GHz hardwired
        eind = (_int64) (tijd - 5) * 1000000 + t;
        while (readTSC () < eind)
        {
            tdStart (0x1D1E);
            Sleep (1);
            tdStop (7);
            // blijkbaar scheduled windows 98 met ticken van 5 ms
            // dus typically duurd deze Sleep 5ms, maar kan ook korter
            // zijn
            // door met de korts mogelijke eenheid te kijken kom
            // toch vrij precies uit.
        }
    }
    else
    {
        Sleep (tijd);
    }
#else


    struct timespec tv;
    /* Construct the timespec from the number of whole seconds... */
    tv.tv_sec = 0;
    /* ... and the remainder in nanoseconds. */
    tv.tv_nsec = tijd * 1000000;
    while (1)
    {
        /* Sleep for the time specified in tv. If interrupted by a
         signal, place the remaining time left to sleep back into tv. */
        int rval = nanosleep (&tv, &tv);
        if (rval == 0)
            /* Completed the entire sleep time; all done. */
            return;
        else if (errno == EINTR)
            /* Interrupted by a signal. Try again. */
            continue;
        else
            /* Some other error; bail out. */
            return;
    }



#endif

}

BaanWT::BaanWT(IMessage &msg, IBaanMessage &baanMessage, IWissels &wissels, ITd &td, BaanInfo_t *baanInfo):
    mMessage(msg),
    mBaanMessage(baanMessage),
    mWissels(wissels),
    mTd(td),
    pInfo(baanInfo)
{
    tdaanStuur = mTd.tdCreate("aanstuur");
    tdwt = mTd.tdCreate ("wt");

    pInfo->tdProgInternal = mTd.tdValueString ("Internal");
    pInfo->tdProgBlok = mTd.tdValueString ("Blok");
    pInfo->tdProgIO = mTd.tdValueString ("IO");
    pInfo->tdProgTimer = mTd.tdValueString ("Timer");
    pInfo->tdProgMuis = mTd.tdValueString ("Muis");
    pInfo->tdProgUserDefined = mTd.tdValueString ("UserDefined");
}


void BaanWT::StartWorker(void)
{
    workerThread = new boost::thread(boost::bind(&BaanWT::BaanWorkerThreadProc, this));
                                             //boost::ref(*this)));
}

void BaanWT::StopWorker(void)
{
    pInfo->StopExecution = 1;
    workerThread->join();
}

void BaanWT::BaanSimulatie ()
{
    int i;
    static int Position[MAX_AANTAL_REGELAARS];

    /* Doe eerst de regelaars aftesten */
    for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
    {
        if (pInfo->RegelArray[i].Gebruikt == 0)
        {
            Position[i] = 0;
        }
        else
        {
            int Snelheid, Detect, adres;
            BlokPointer_t *pBlok;

            pBlok = pInfo->RegelArray[i].pKopBlok;
            adres = pBlok->pBlok->hardwareAdres;
            Snelheid = pInfo->SimulatieArray[adres].snelheid;
            if (pBlok->pBlok->State == BLOK_VOORUIT)
            {
                pInfo->SimulatieArray[adres].returnWaarde = -Snelheid + 0x7f;
            }
            else
            {
                pInfo->SimulatieArray[adres].returnWaarde = Snelheid + 0x7f;
            }
            Position[i] += Snelheid >> 3;
            Detect = 0;
            if (Position[i] > pBlok->Lengte)
            {
                Position[i] = Position[i] - pBlok->Lengte;
                Detect = 1;
            }
            pBlok = pBlok->blokRicht[pBlok->pBlok->State - BLOK_VOORUIT];
            if (pBlok->pBlok->RegelaarNummer == i)
            {
                // blok is van ons
                // Detectie blok kijk of we een signaal terug moeten geven of niet

                adres = pBlok->pBlok->hardwareAdres;
                if (Detect)
                {
                    pInfo->SimulatieArray[adres].detect = Detect;
                }
                if (pInfo->SimulatieArray[adres].detect == 0)
                {
                    // geen detectie dus 0x7f terug
                    Snelheid = 0;
                }
                pInfo->SimulatieArray[adres].returnWaarde = Snelheid + 0x7f;
            }
        }
    }
    for (i = 0; i < MAX_MICRO_MEM; i++)
    {
        int adres, data, snelheid;

        adres = pInfo->centraleMicroMem[2 * i];
        data = pInfo->centraleMicroMem[2 * i + 1];
        if (adres == -1)
            continue;
        adres |= (data & 3) << 8;
        data >>= 2;
        if (adres == 0x3ff)
            continue;
        if (adres < MAX_NOBLOKS)
        {
            pInfo->centraleMicroMem[2 * i + 1] =
                    pInfo->SimulatieArray[adres].returnWaarde;
            pInfo->SimulatieArray[adres].returnWaarde = 0x7f;     // clear de return waarde
            if ((data == pInfo->Vooruit) || (data == pInfo->Achteruit))
            {
                pInfo->SimulatieArray[adres].richting = data;
                pInfo->SimulatieArray[adres].detect = 0;
            }
            else
            {
                switch (data)
                {
                case HW_SEIN_GROEN:
                    pInfo->SimulatieArray[adres].SeinState = SEIN_GROEN;
                    break;
                case HW_SEIN_ROOD:
                    pInfo->SimulatieArray[adres].SeinState = SEIN_ROOD;
                    break;
                case HW_SEIN_GEEL:
                    pInfo->SimulatieArray[adres].SeinState = SEIN_GEEL;
                    break;
                default:
                    pInfo->SimulatieArray[adres].snelheid = data;
                }
            }
            // even de lamp brush misbruiken
            snelheid = pInfo->SimulatieArray[adres].snelheid;
            if (pInfo->SimulatieArray[adres].richting == pInfo->Achteruit)
            {
                snelheid = -snelheid;
            }

            switch (pInfo->SimulatieArray[adres].SeinState)
            {
            case SEIN_GROEN:
                //                DisplaySnelheid(pInfo->pDC, 1,&pInfo->WitteLampBrush, &pInfo->ResetGroenBrush, 200, adres*8,
                //                                      -40,40,snelheid);
                break;
            case SEIN_ROOD:
                //                DisplaySnelheid(pInfo->pDC, 1,&pInfo->WitteLampBrush, &pInfo->ResetRoodBrush, 200, adres*8,
                //                                      -40,40,snelheid);
                break;
            case SEIN_GEEL:
                //                DisplaySnelheid(pInfo->pDC, 1,&pInfo->WitteLampBrush, &pInfo->LampBrush, 200, adres*8,
                //                                      -40,40,snelheid);
                break;
            case SEIN_VAST_GROEN:
            case SEIN_VAST_GEEL:
            case SEIN_VAST_ROOD:
                break; // nothing defined
            }
        }
    }
}



void BaanWT::RegelAanvraag (void)
{
    int Accept = 0;               /* default niet geaccepteerd */
    if (pInfo->RegelaarAanvraag.NieuweRegelaar)
    {
        int Lengte, RegelNummer;
        int vt, nvt;
        BlokPointer_t *pBlokNummer, *pBlok;

        pBlokNummer = &pInfo->BlokPointer[pInfo->RegelaarAanvraag.BlokNummer];
        Lengte = pInfo->RegelaarAanvraag.Lengte;
        vt = pInfo->RegelaarAanvraag.Richting;
        nvt = (vt + 1) & 1;       // inverse richting
        RegelNummer = pInfo->RegelaarAanvraag.RegelaarNummer;
        pBlok = pBlokNummer;
        if (pBlokNummer->pBlok->State == 0)
        {
            int Total = 0;
            /*
           ** check of er voldoende blokken vrij zijn
           **/
            if (pInfo->RegelaarAanvraag.NeemKopBlokMee)
            {
                Total = pBlok->Lengte;
            }
            Accept = 1;           /* ik hoop dat die geaccepteerd wordt */

            while (Total <= Lengte)
            {
                pBlok = pBlok->blokRicht[nvt];
                Total += pBlok->Lengte;
                if (pBlok->pBlok->State != 0)
                {
                    /* Blok bezet dus kan het niet */
                    Accept = 0;
                    Total = Lengte + 1;   /* break the loop */
                }
            }

            if (Accept)
            {
                int State;
                /*
               ** Er zijn voldoende blokken vrij.
               ** Dus we beleggen ze voor deze regelaar.
               **/
                if (vt)
                {
                    State = BLOK_ACHTERUIT;
                }
                else
                {
                    State = BLOK_VOORUIT;
                }

                pBlokNummer->pBlok->State = State;
                pBlokNummer->pBlok->RegelaarNummer = RegelNummer;
                pBlokNummer->pBlok->Snelheid = 0;
                while (pBlok != pBlokNummer)
                {
                    pBlokNummer = pBlokNummer->blokRicht[nvt];
                    pBlokNummer->pBlok->State = State;
                    pBlokNummer->pBlok->RegelaarNummer = RegelNummer;
                    pBlokNummer->pBlok->Snelheid = 0;
                }
            }
        }
        pInfo->RegelaarAanvraag.NieuweRegelaar = 0;
    }
    if (pInfo->RegelaarAanvraag.VerwijderRegelaar)
    {
        int Regel;
        BlokPointer_t *pBlok;

        Regel = pInfo->RegelaarAanvraag.RegelaarNummer;
        pBlok = pInfo->RegelArray[Regel].pKopBlok;
        /*
       ** Omdat we niet weten of de richting die
       ** gegeven is goed is testen we gewoon beiden
       ** kanten
       **/
        pBlok->pBlok->State = BLOK_VRIJ;
        pBlok->pBlok->RegelaarNummer = -1;
        mBaanMessage.Post (WM_BLOK_DISPLAY, pBlok->pBlok - pInfo->Blok, Regel,
                           BLOK_CLEAR);

        Accept = 0;
        do
        {
            pBlok = pBlok->pVorigBlok;
            if ((pBlok->pBlok->RegelaarNummer == Regel) &&
                    (pBlok->pBlok->State != BLOK_VRIJ))
            {
                pBlok->pBlok->State = BLOK_VRIJ;
                pBlok->pBlok->RegelaarNummer = -1;
                mBaanMessage.Post (WM_BLOK_DISPLAY, pBlok->pBlok - pInfo->Blok,
                                   Regel, BLOK_CLEAR);
            }
            else
                Accept = 1;
        }
        while (Accept == 0);
        pBlok = pInfo->RegelArray[Regel].pKopBlok;
        Accept = 0;
        do
        {
            pBlok = pBlok->pVolgendBlok;
            if ((pBlok->pBlok->RegelaarNummer == Regel) &&
                    (pBlok->pBlok->State != BLOK_VRIJ))
            {
                pBlok->pBlok->State = BLOK_VRIJ;
                pBlok->pBlok->RegelaarNummer = -1;
                mBaanMessage.Post (WM_BLOK_DISPLAY, pBlok->pBlok - pInfo->Blok,
                                   Regel, BLOK_CLEAR);
            }
            else
                Accept = 1;
        }
        while (Accept == 0);
        pInfo->RegelaarAanvraag.VerwijderRegelaar = 0;
    }
    pInfo->RegelaarAanvraag.AanvraagAccept = Accept;
    pInfo->RegelaarAanvraag.Aanvraag = 0;
    baanSemaphoreUp (pInfo->RegelaarAanvraag.semWacht);
}

// in principe doet dit hetzelfde als zendUsbBuffer
// maar in het geval van simulatie zorgt dit voor
// de informatie verzameling van de simulator
int BaanWT::localZendUsbBuffer (unsigned char *buffer,
                                unsigned char *bufferOnt, int lengte, int *stopBit)
{
    if (pInfo->Simulatie)
    {
        int i;
        lengte /= 2;              // lengte is in bytes
        // clear de adres info
        for (i = 0; i < MAX_MICRO_MEM; i++)
        {
            pInfo->centraleMicroMem[2 * i] = -1;  // alleen de adres waarde op -1 niet de returnwaarde
        }
        // haal de return waardes op
        for (i = 0; i < lengte; i++)
        {
            bufferOnt[i] = pInfo->centraleMicroMem[2 * i + 1];
        }
        for (i = 0; i < lengte; i++)
        {
            pInfo->centraleMicroMem[2 * i] = buffer[2 * i];
            pInfo->centraleMicroMem[2 * i + 1] = buffer[2 * i + 1];
        }
        BaanSimulatie ();
        *stopBit = 0;
        ericSleep (CYCLETIME);    // de echte hardware wacht
        return lengte;
    }
    else
    {
        return zendUsbBuffer (buffer, bufferOnt, lengte, stopBit);
    }
}

void BaanWT::geefIdleBytes (int *byte1, int *byte2)
{
    int i;
    int c;

    // het programma moet soms extra data zenden zodat de vorige
    // data opgehaald kan worden. We moeten voor deze idle data
    // wel 1 aanwezige controller aansturen anders krijgen we
    // niet alle data terug.
    for (c = 1; c < MAX_NOBLOKS; c++)
    {
        i = pInfo->idleBlokCounter++;
        if (i >= MAX_NOBLOKS)
        {
            i = 1;
            pInfo->idleBlokCounter = i;
        }
        if (pInfo->BlokPointer[i].BlokIONummer != -1)
        {
            // Gebruikt in de database
            if (BLOK_VRIJ == pInfo->Blok[i].State)
            {
                // compenseer voor 10 bit adres en 6 bit data
                *byte2 = (pInfo->Blok[i].hardwareAdres >> 8);
                *byte1 = pInfo->Blok[i].hardwareAdres & 0xff;
                return;

            }
        }
    }
    // eigenlijk zou er een blok vrij moeten zijn maar blijkbaar niet
    // dan maar aansturen op snelheid
    // weet niet od dat goed gaat maar met mijn huidige baan is dit niet
    // te controlleren
    // TODO: check dit!
    for (c = 1; c < MAX_NOBLOKS; c++)
    {
        i = pInfo->idleBlokCounter++;
        if (i >= MAX_NOBLOKS)
        {
            i = 1;
            pInfo->idleBlokCounter = i;
        }
        if (pInfo->BlokPointer[i].BlokIONummer != -1)
        {
            // Gebruikt in de database
            // compenseer voor 10 bit adres en 6 bit data
            *byte2 =
                    (pInfo->Blok[i].Snelheid << 2) | (pInfo->
                                                      Blok[i].hardwareAdres >> 8);
            *byte1 = pInfo->Blok[i].hardwareAdres & 0xff;
            return;
        }
    }
    // Ik moet wel iets terug geven dus dan maar iets
    // wat niets kan verstoren namelijk het baan idle
    // patroon. Ik krijg alleen dan wel een melding
    // dat niet alles ontvangen is maar dat is beter
    // dan ongeinitializeerde data.
    *byte1 = 0xff;
    *byte2 = 0xff;
    mMessage.message ("Idlebytes niet gevonden in geefIdleBytes");
}



// return waardes
// 0 = ok
// 1 = timeout
// 2 = Van de vorige transfer heb ik niet alle antwoorden
int BaanWT::BaanAansturing (hardwareCom_t * huidigeArray,
                            hardwareCom_t * vorigeArray)
{
    int i, aantal, cx, stop;
    unsigned char bufferIn[256], bufferOut[256];

    mTd.tdStart (tdaanStuur);

    // als we nu minder sturen dan vorige keer dan sturen we nu meer
    aantal = huidigeArray->aantal;
    if (aantal < vorigeArray->aantal)
    {
        aantal = vorigeArray->aantal;
    }

    // eerst gaan we het transmissie buffer vullen
    for (i = 0; i < aantal; i++)
    {
        int byte1, byte2;
        hardwareArray_t *parray;

        if (i >= huidigeArray->aantal)
        {
            geefIdleBytes (&byte1, &byte2);
            // de return waardes worden niet opgehaald in de code
        }
        else
        {
            parray = &huidigeArray->array[i];
            byte1 = parray->adres;
            if (parray->adres == -1)
            {
                // een ongebruikte locatie
                geefIdleBytes (&byte1, &byte2);

                // initalizeer de rest van deze index zodat we
                // zeker zijn dat er geen troep blijft hangen
                parray->nummer = -1;
                parray->data = 0;
                parray->returnGewenst = 0;
                parray->blokIO = HW_BLOK;
            }
            else
            {
                byte2 = parray->data;
                // compenseer voor 10 bit adres en 6 bit data
                byte2 = (byte2 << 2) | (byte1 >> 8);
                byte1 = byte1 & 0xff;
            }
            parray->adres = -1;   // zet elke geleze locatie meteen op ongebruikt
        }
        bufferOut[i * 2] = byte1;
        bufferOut[i * 2 + 1] = byte2;
    }

    // transmissie buffer is gevult nu kunnen we communiceren
    cx = localZendUsbBuffer (bufferOut, bufferIn, 2 * aantal, &stop);
    if (cx < vorigeArray->aantal)
    {
        // we hebben minder terug gekregen dan verwacht dus exit deze loop
        // en trigger een reset hardware
        std::cout << "received " << cx << " verwacht " << vorigeArray->aantal << std::endl;
        return 2;
    }



    // check voor een stop conditie
    if (stop || (pInfo->noodStop))
    {
        // alleen een hoog naar laag transitie is interresant
        if (pInfo->vorigeStopStatus == 1)
        {

            // hebbes een hoog laag transitie
            pInfo->Stop = (pInfo->Stop + 1) & 1;
            if (pInfo->Stop)
            {
                mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                                   pInfo->EindBlok.XCoord +
                                   (pInfo->EindBlok.YCoord << 16),
                                   RESET_SIZE + (RESET_SIZE << 16), IMainWindowDrawing::Red);
            }
            else
            {
                for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
                {
                    if (pInfo->RegelArray[i].Gebruikt != 0)
                    {
                        pInfo->RegelArray[i].RegelaarStopResume ();
                    }
                }
                mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                                   pInfo->EindBlok.XCoord +
                                   (pInfo->EindBlok.YCoord << 16),
                                   RESET_SIZE + (RESET_SIZE << 16), IMainWindowDrawing::Green);
            }
            // De regelaar op 0 stellen doet de baanDetection (continue)
        }
        pInfo->vorigeStopStatus = 0;
        pInfo->noodStop = 0;      // reset
    }
    else
    {
        pInfo->vorigeStopStatus = 1;
    }
    aantal = vorigeArray->aantal;
    // Nu de return array processen
    for (i = 0; i < aantal; i++)
    {
        int returnWaarde;
        hardwareArray_t *parray;

        Blok_t *pBlok;
        parray = &vorigeArray->array[i];
        returnWaarde = bufferIn[i];
        // test voor kortsluiting altijd!
        if (parray->blokIO == HW_BLOK)
        {
            if ((returnWaarde == 0) && (parray->nummer > 0) &&
                    (parray->nummer < MAX_NOBLOKS))
            {
                // omdat parray->nummer in de gewenste range zit mag ik de
                // pointer referencen
                pBlok = &pInfo->Blok[parray->nummer];
                // Er is kortsluiting
                if (pBlok->RegelaarNummer >= 0)
                {
                    int regelNummer;

                    // en er is een regelaar
                    regelNummer = pBlok->RegelaarNummer;
                    pInfo->RegelArray[regelNummer].KortSluiting ();
                    mBaanMessage.Post (WM_BLOK_DISPLAY, pBlok - pInfo->Blok,
                                       regelNummer, BLOK_KORTSLUIT);
                }
            }
        }

        if (parray->returnGewenst)
        {
            parray->returnGewenst = 0;
            if (parray->blokIO == HW_BLOK)
            {
                Blok_t *pBlok;

                pBlok = &pInfo->Blok[parray->nummer];
                if (returnWaarde == 0)
                {
                    // er is kortsluiting maar die is al afgehandeld
                    pBlok->hardwareReturnWaarde = pBlok->nulWaarde;
                }
                else
                {
                    pBlok->hardwareReturnWaarde = returnWaarde;
                }
            }
            else
            {
                IOBits_t *pIOBits;

                pIOBits = pInfo->IOBits[parray->nummer].get();
                pIOBits->hardwareReturnWaarde = returnWaarde;
            }
        }
    }
    mTd.tdStop (tdaanStuur);

    return 0;
}

void BaanWT::initSimulatieArray ()
{
    int i;

    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        pInfo->SimulatieArray[i].SeinState = SEIN_GROEN;
        pInfo->SimulatieArray[i].richting = pInfo->Vooruit;
        pInfo->SimulatieArray[i].snelheid = 0;
        pInfo->SimulatieArray[i].detect = 0;
        pInfo->SimulatieArray[i].returnWaarde = 0x7f;
    }
}


void BaanWT::StartHardware ()
{
    int i;
    if (pInfo->Simulatie == 0)
    {
        unsigned char bufferIn[64], bufferOut[64];
        int cx, byte1, byte2, stop;
        // na het openen gaan we een bericht sturen en als ik antwoord
        // krijg kan ik met de baan praten
        if (openUsbPort ())
        {
            // simulatie als ik een open error krijg
            pInfo->Simulatie = 1;
        }
        else
        {

            geefIdleBytes (&byte1, &byte2);

            bufferOut[0] = byte1;
            bufferOut[1] = byte2;
            cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
            if (cx < 0)
            {
                // error in usb connectie
                pInfo->Simulatie = 1;

            }
            else
            {
                // zend nogmaals 2 maal om zeker tezijn dat de baan in sync is
                // De controllers gaan eerst sync zoeken en dan pas gaan ze naar
                // berichten luisteren. De sync wordt elke 128 bytes gestuurd
                cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
                cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
                bufferOut[0] = 0xff;
                bufferOut[1] = 0xff;
                cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
                if (cx != 1)
                {
                    // treinbaan waarschijnlijk niet aan
                    pInfo->Simulatie = 1;
                }
                else
                {
                    pInfo->Simulatie = 0;
                }
            }
        }
    }

    printf ("start hw in %s\n", (pInfo->Simulatie == 1) ? "sim" : "hw");
    pInfo->hardwareSnelheid.aantal = 0;
    pInfo->hardwareDetection.aantal = 0;

    if (pInfo->Simulatie != 0)
    {
        initSimulatieArray ();
    }

    // nu de controllers gereset zijn, zijn ze wel de rij info kwijt
    // waardoor een trein plotseling de verkeerde kant kan oprijden
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        pInfo->Blok[i].hardwareSeinState = SEIN_GROEN;
        pInfo->Blok[i].hardwareState = BLOK_VRIJ;
        pInfo->Blok[i].Snelheid = 0;      // om er zeker van te zijn dat die
        //  geupdate wordt Snelheid ook = 0
    }


}

void BaanWT::StopHardware (void)
{

    if (pInfo->Simulatie == 0)
    {
        closeUsbPort ();
    }
}


void BaanWT::ResetHardware ()
{
    if (pInfo->Simulatie == 0)
    {
        StopHardware ();
        ericSleep (2000);
        StartHardware ();
    }
}





/*
** Controleer of er blokken vrijgegeven moeten worden bij de huidige
** treinlengte
**/
void BaanWT::BaanCheckLengte (int RegelNummer, int NeemKopBlokMee)
{
    int Total = 0;
    int State;
    int Regel;
    int Lengte;
    BlokPointer_t *pBlok, *pKopBlok;

    Lengte = pInfo->RegelArray[RegelNummer].Lengte;
    pKopBlok = pInfo->RegelArray[RegelNummer].pKopBlok;

    State = pKopBlok->pBlok->State;
    pBlok = pKopBlok;

    if (NeemKopBlokMee)
    {
        Total = pBlok->Lengte;
    }

    if (State == BLOK_VOORUIT)
    {
        pBlok = pBlok->pVorigBlok;
    }
    else
    {
        pBlok = pBlok->pVolgendBlok;
    }
    do
    {
        /*
       ** Omdat we zeker moeten zijn dat de rest ook binnen
       ** het blok valt verhogen we later.
       **/
        if (Total >= Lengte)
        {
            GeefBlokVrij (pBlok);
        }
        Total += pBlok->Lengte;
        if (State == BLOK_VOORUIT)
        {
            pBlok = pBlok->pVorigBlok;
        }
        else
        {
            pBlok = pBlok->pVolgendBlok;
        }

        Regel = pBlok->pBlok->RegelaarNummer;
    }
    // alleen meerdere blokken vrijgeven als de laatste wagon geen condensator heeft
    while ((Regel == RegelNummer) && (pKopBlok != pBlok));
}

/* Aangenomen word dat de regelaar gebruikt is ! */
void BaanWT::SetSnelheid (int Regelaar, int Snelheid)
{
    int lengte;
    int Regel;
    int Richting;
    BlokPointer_t *pBlok, *pStartBlok;
    hardwareArray_t array;

    pBlok = pInfo->RegelArray[Regelaar].pKopBlok;

    if ((pBlok->pBlok->Snelheid == 0) && (Snelheid == 0))
    {
        // als de snelheid 0 is en we hebben al 0 verstuurd dan
        // doe ik niets meer en kan deze plek voor andere data gebruikt worden
        array.adres = -1;
        array.returnGewenst = 0;
        // forceer de return waarde op 0 volt
        pBlok->pBlok->hardwareReturnWaarde = pBlok->pBlok->nulWaarde;
    }
    else
    {
        // normale snelheid case stuur dus altijd aan
        // todo check of we dit kunnen decimeren als we later
        // de snelheids regeling hebben geimplementeerd
        array.data = Snelheid;
        array.returnGewenst = 1;
        if (pBlok->pBlok->State != pBlok->pBlok->hardwareState)
        {
            pBlok->pBlok->hardwareState = pBlok->pBlok->State;
            array.returnGewenst = 0;

            if (pBlok->pBlok->State == BLOK_VOORUIT)
            {
                array.data = pInfo->Vooruit;
            }
            else
            {
                array.data = pInfo->Achteruit;
            }
        }
        array.adres = pBlok->pBlok->hardwareAdres;
    }
    array.nummer = array.adres;
    array.blokIO = HW_BLOK;

    // altijd het kop blok aansturen
    pInfo->hardwareSnelheid.array[Regelaar] = array;

    if (pBlok->pBlok->State == BLOK_VOORUIT)
    {
        Richting = 1;
    }
    else
    {
        Richting = 0;
    }
    pBlok->pBlok->Snelheid = Snelheid;

    Snelheid =
            pInfo->RegelArray[Regelaar].achterLiggendeBlokken (Snelheid);

    // doe nu alleen maar de achterliggende blokken
    lengte = 0;
    pStartBlok = pBlok;           // wordt voor de eind conditie gebruikt voor als we ons zelf tegenkomen
    do
    {
        BlokPointer_t *pBlokNext;

        if (Richting)
        {
            // Richting is VOORUIT maar ik wil
            // naar achteren dus neem vorig blok
            pBlok = pBlok->pVorigBlok;
        }
        else
        {
            pBlok = pBlok->pVolgendBlok;
        }
        Regel = pBlok->pBlok->RegelaarNummer;
        if (Regel == Regelaar)
        {
            // Dit blok behoord bij deze regelaar
            // Kijk eerst of we de juiste richting opgaan
            if (pBlok->pBlok->State != pBlok->pBlok->hardwareState)
            {
                if (pBlok->pBlok->State == BLOK_VOORUIT)
                {
                    array.data = pInfo->Vooruit;
                }
                else
                {
                    array.data = pInfo->Achteruit;
                }
                array.returnGewenst = 0;  // geen return hiervan

                array.adres = pBlok->pBlok->hardwareAdres;
                array.nummer = array.adres;
                if (pInfo->hardwareLaag.nieuwItem (array) == 0)
                {
                    // als het gelukt is dan de state veranderen
                    pBlok->pBlok->hardwareState = pBlok->pBlok->State;
                    // ervoor zorgen dat er geen junk in de return waarde staat
                    pBlok->pBlok->hardwareReturnWaarde =
                            pBlok->pBlok->nulWaarde;
                }
            }
            array.returnGewenst = 1;      // voor snelheid dinen wel een return gewenst
            // vanwege de laatste wagon detectie
            if (pBlok->pBlok->Snelheid != Snelheid)
            {
                // alleen aanpassen als het blok van deze regelaar is en de snelheid
                // is verschillend
                array.adres = pBlok->pBlok->hardwareAdres;
                array.nummer = array.adres;
                array.data = Snelheid;
                if (pInfo->hardwareLaag.nieuwItem (array) == 0)
                {
                    pBlok->pBlok->Snelheid = Snelheid;
                    // ervoor zorgen dat er geen junk in de return waarde staat
                    pBlok->pBlok->hardwareReturnWaarde =
                            pBlok->pBlok->nulWaarde;
                }
            }
            // de laatste wagon checking
            if (Richting)
            {
                // Richting is VOORUIT maar ik wil
                // naar achteren dus neem vorig blok
                pBlokNext = pBlok->pVorigBlok;
            }
            else
            {
                pBlokNext = pBlok->pVolgendBlok;
            }
            if (Snelheid == 0)
            {
                // De eerste 2 return waardes voor een laatste wagon ingeval
                // van een 0 spanning zijn onbruikbaar
                pInfo->RegelArray[Regelaar].laatsteWagonDetect = -2;
                pInfo->RegelArray[Regelaar].laatsteWagonDecimatie = 0;
            }
            if ((pBlokNext->pBlok->RegelaarNummer != Regelaar) &&
                    (pInfo->RegelArray[Regelaar].laatsteWagonCheck) &&
                    (Snelheid != 0))
            {
                // we zitten in het laatste blok en deze trein heeft
                // condensatoren in de laatste wagon
                pInfo->RegelArray[Regelaar].laatsteWagonDecimatie += 1;
                if (pInfo->RegelArray[Regelaar].laatsteWagonDecimatie >=
                        LAATSTE_WAGON_DECIMATIE)
                {
                    int Spanning, Detect;
                    int detectDrempel;


                    pInfo->RegelArray[Regelaar].laatsteWagonDecimatie = 0;

                    Spanning = pBlok->pBlok->hardwareReturnWaarde;
                    Spanning = Spanning - pBlok->pBlok->nulWaarde;
                    Detect = 0;
                    // als de spanning hoog is dan heb je last van wat rest lading
                    // waardoor je verkeerde detecties krijgt
                    // Bij stille condities heb je hier geen last van maar
                    // als er gereden wordt blijkt er toch iets aan de hand te zijn
                    // Een remedie is om de drempel te verhogen voor dit geval.
                    if (Snelheid > 40)
                    {
                        // tja zomaar wat geprobeerd blijkt leuk te werken voor
                        // de V215
                        detectDrempel = 2 * DETECT_DREMPEL;
                    }
                    else
                    {
                        detectDrempel = DETECT_DREMPEL;
                    }
                    if ((Spanning < -detectDrempel)
                            || (Spanning > detectDrempel))
                    {
                        Detect = 1;
                    }
                    if (Detect == 0)
                    {
                        // ik zie de laatste wagon niet dus tel dit
                        pInfo->RegelArray[Regelaar].laatsteWagonDetect +=
                                1;
                        pInfo->RegelArray[Regelaar].
                                laatsteWagonCheckTotal += 1;
                        if (pInfo->RegelArray[Regelaar].
                                laatsteWagonDetect >=
                                AANTAL_LAATSTE_WAGON_DETECTIES)
                        {
                            pInfo->RegelArray[Regelaar].
                                    laatsteWagonDetect = 0;
                            if (lengte >
                                    pInfo->RegelArray[Regelaar].Lengte)
                            {
                                // geeft dit blok vrij en geef meteen een opdracht om
                                // het volgende blok te meten.
                                GeefBlokVrij (pBlok);
                                pInfo->RegelArray[Regelaar].laatsteWagonDecimatie = LAATSTE_WAGON_DECIMATIE - 1;    // volgend blok meteen meten
                                pBlok =
                                        pBlok->blokRicht[pInfo->
                                        RegelArray[Regelaar].
                                        Richting];
                                lengte -= pBlok->Lengte;  // voorkom dat we hem 2 maal tellen
                            }
                            // else
                            // zouden we iets kunnen doen met de laaste wagon niet aanwezig
                            // dit is verwijdered (LaatsteWagonNietAanwezig). Bleek
                            // tevaak voor te komen
                        }
                    }
                    else
                    {
                        // We detecteren een wagon
                        if (lengte >
                                (pInfo->RegelArray[Regelaar].Lengte + 100))
                        {
                            // De trein is nu 1m verder en we hebben nog steeds geen
                            // detectie dat de wagon weg is dus laten we de trein stoppen
                            pInfo->RegelArray[Regelaar].
                                    LaatsteWagonStaatStil ();
                        }
                        // ik zie de laatste wagon even kijken of we hem al eens gemist hebben
                        if (pInfo->RegelArray[Regelaar].
                                laatsteWagonDetect > 0)
                        {
                            // valse detectie misschien vuil?
                            pInfo->RegelArray[Regelaar].
                                    laatsteWagonCheckError += 1;
                            pInfo->RegelArray[Regelaar].
                                    laatsteWagonDetect = 0;
                            pInfo->RegelArray[Regelaar].
                                    aantalFouten[pBlok->pBlok->hardwareAdres] +=
                                    1;
                        }
                    }
                    if ((pInfo->RegelArray[Regelaar].
                         laatsteWagonCheckTotal > 20)
                            &&
                            ((pInfo->RegelArray[Regelaar].
                              laatsteWagonCheckTotal >> 3) <
                             pInfo->RegelArray[Regelaar].
                             laatsteWagonCheckError))
                    {
                        // de wagon is vuil
                        pInfo->RegelArray[Regelaar].LaatsteWagonVuil ();
                    }


                    if (pBlok != pStartBlok)
                    {
                        // als het niet het start blok is dan voer een meting uit
                        // Als het wel het start blok is dan wordt de meting zowiezo
                        // al gedaan
                        array.adres = pBlok->pBlok->hardwareAdres;
                        array.nummer = array.adres;
                        array.data = Snelheid;
                        if (pInfo->hardwareLaag.nieuwItem (array) == 0)
                        {
                            pBlok->pBlok->Snelheid = Snelheid;
                        }
                        else
                        {
                            // de aanvraag is niet gehonoreerd dus probeer ik het de volgende
                            // keer weer.
                            pInfo->RegelArray[Regelaar].
                                    laatsteWagonDecimatie =
                                    LAATSTE_WAGON_DECIMATIE - 1;
                        }
                    }
                }
            }
        }
        // dit blok zijn we door dus telop
        lengte += pBlok->Lengte;
    }
    while ((Regel == Regelaar) && (pStartBlok != pBlok));
}


// Geef dit aangewezen blok vrij en kleur het blok wit
void BaanWT::GeefBlokVrij (BlokPointer_t * pBlok)
{
    hardwareArray_t array;

    mBaanMessage.Post (WM_BLOK_DISPLAY, pBlok->pBlok - pInfo->Blok,
                       pBlok->pBlok->RegelaarNummer, BLOK_CLEAR);


    pBlok->pBlok->State = BLOK_VRIJ;
    pBlok->pBlok->RegelaarNummer = -1;
    pBlok->pBlok->Snelheid = 0;
    pBlok->pBlok->hardwareReturnWaarde = pBlok->pBlok->nulWaarde;

    array.blokIO = HW_BLOK;
    array.adres = pBlok->pBlok->hardwareAdres;
    array.data = 0;               // snelheid 0
    // ik wil niets terug horen
    array.nummer = array.adres;
    array.returnGewenst = 0;
    // TODO als er veel delay is kan het 0 zetten laat aankomen en kan het blok
    // al onder iemand anders zijn controlle zitten
    // misschien moet is snelheid =0 en hardwareState in het aansturen van de
    // hardware zetten???? Of onder hoge poiriteit afwerken??
    // Misschien toch in de aansturing de hardware state zetten?
    // liever eigenlijk van de state afblijven in de aansturing misschien alleen
    // de snelheid
    if (pInfo->hardwareLaag.nieuwItem (array) == 0)
    {
        // Alleen als het commando verzonden is mag ik aan de state komen
        // anders laten we het blok maar aan staan
        pBlok->pBlok->hardwareState = BLOK_VRIJ;
        pBlok->pBlok->Snelheid = 0;
    }
}

// aanvraag om een blok vrij te geven
// Een blok wordt alleen vrijgegeven als het het achterste
// blok van een trein is
// Er wordt geen rekening gehouden met de lengte van de trein
// maar er blijft minimaal altijd een blok over (het kopblok)
// Ik test ook of het regelaarnummer correct is want anders kunnen
// we een trein met alleen 1 blok die achter een andere trein
// staat verwijderen.
void BaanWT::BaanGeefVrij (int BlokNummer)
{
    int regelaar, tegenRichting;
    BlokPointer_t *pBlok, *pNieuwBlok;

    // we kijken niet direct of dit blok vrij gegeven kan worden,
    // maar we pakken de regelaar en gaan vanaf het kopblok
    // terug lopen naar het laatste blok van die regelaar.
    // Als we op het laatste blok zitten dan kijken we of dat
    // het gevraagde blok is en zo ja geeft die vrij

    pBlok = &pInfo->BlokPointer[BlokNummer];
    regelaar = pBlok->pBlok->RegelaarNummer;
    if (regelaar >= 0)
    {

        // We gaan eerst het laatste blok zoeken van deze regelaar
        tegenRichting = pInfo->RegelArray[regelaar].Richting ^ 1;
        pNieuwBlok = pInfo->RegelArray[regelaar].pKopBlok;
        do
        {
            pBlok = pNieuwBlok;
            pNieuwBlok = pBlok->blokRicht[tegenRichting];
        }
        while (pNieuwBlok->pBlok->RegelaarNummer == regelaar);

        // pBlok wijst naar het laatste blok van deze regelaar
        // test ook of pBlok geen kopBlok is
        if ((pBlok->pBlok == &(pInfo->Blok[BlokNummer])) &&
                (pInfo->RegelArray[regelaar].pKopBlok != pBlok))
        {
            GeefBlokVrij (pBlok);
        }
    }
}


// Precondition dat de hardware array al goed staat wij vullen alleen adres en data in
void BaanWT::zetSein (BlokPointer_t * pBlok, hardwareArray_t * array,
                      SeinState_t sein)
{
    // als er geen sein is dan doen we niets
    if (pBlok->pBlok->blokSein == GEEN_SEIN)
        return;

    // bij een rood groen sein wordt geel omgezet naar groen
    if ((pBlok->pBlok->blokSein == ROOD_GROEN_SEIN) && (sein == SEIN_GEEL))
    {
        sein = SEIN_GROEN;
    }

    if (pBlok->pBlok->hardwareSeinState != sein)
    {
        array->adres = pBlok->pBlok->hardwareAdres;
        array->nummer = array->adres;
        switch (sein)
        {
        case SEIN_GROEN:
        case SEIN_VAST_GROEN:
            array->data = HW_SEIN_GROEN;
            break;
        case SEIN_ROOD:
        case SEIN_VAST_ROOD:
            array->data = HW_SEIN_ROOD;
            break;
        case SEIN_GEEL:
        case SEIN_VAST_GEEL:
            array->data = HW_SEIN_GEEL;
            break;
        default:
            assert (0);
        }
        if (pInfo->hardwareLaag.nieuwItem (*array) == 0)
        {
            // sein wordt omgezet dus update het seinState veld
            pBlok->pBlok->hardwareSeinState = sein;
        }
    }
}

// bereid de seinState voor. Als een regelaar b.v het sein op rood zet en later
// een ander hetzelfde blok dat sein op groen zet blijft rood bestaan
void BaanWT::prepareSein (BlokPointer_t * pBlok, SeinState_t sein)
{
    if (pBlok->pBlok->seinState < sein)
    {
        pBlok->pBlok->seinState = sein;
    }
}

// Deze routine checkt elke keer alle seinen
// Het princiepe is eerst alle seinen op groen gaan behalve de wissel_rood
// seinen. Dit op groen zetten we doen we aan het einde van de routine
// Dit om wat CPU power te saven want daar moet ik zowiezo door alle blokken
// heen.
// De seinen worden gezet door de kop en staart blokken van elke
// trein even zetten. Hierbij is een additionele regel dat als een
// sein op rood is gezet door een trein dat hetzelfde sein niet door een
// andere trein op groen mag worden gezet. Dit geld natuurlijk ook voor geel.
// Dus promotie naar rood mag maar niet naar groen.

void BaanWT::BaanSetSein ()
{
    int State;
    BlokPointer_t *pBlok, *pKop, *pBlokVolg, *pBlokVolgVolg;
    hardwareArray_t array;
    int Regelaar, blok;
    SeinState_t seinState;

    // dit geld voor alle sein transacties
    array.blokIO = HW_BLOK;
    // ik wil niets terug horen
    array.nummer = 0;
    array.returnGewenst = 0;

    for (Regelaar = 0; Regelaar < MAX_AANTAL_REGELAARS; Regelaar++)
    {

        if (pInfo->RegelArray[Regelaar].Gebruikt != 0)
        {
            // eerst gaan we het sein zetten voor het kopblok en het
            // inrij blok
            pBlok = pInfo->RegelArray[Regelaar].pKopBlok;
            State = pBlok->pBlok->State;
            if (State == BLOK_VOORUIT)
            {
                pKop = pBlok->pVolgendBlok;
                pBlokVolg = pKop->pVolgendBlok;
                if (pBlokVolg == NULL)
                {
                    pBlokVolgVolg = NULL;
                }
                else
                {
                    pBlokVolgVolg = pBlokVolg->pVolgendBlok;
                }

            }
            else
            {
                pKop = pBlok->pVorigBlok;
                pBlokVolg = pKop->pVorigBlok;
                if (pBlokVolg == NULL)
                {
                    pBlokVolgVolg = NULL;
                }
                else
                {
                    pBlokVolgVolg = pBlokVolg->pVorigBlok;
                }
            }
            // Niet in de table opgenomen maar er wordt ook voor de aanwezigheid
            // van bovenleiding gekeken voor de elocs.
            // De onderstaande tabel wordt van boven naar beneden afgewerkt
            //
            //  pBlok   pKop   als    pKop    pBlokVolg  pBlokVolgVolg
            //  Rood     *         !regelnum
            //  Geel    Rood           *     >BLOK_VRIJ
            //  Groen   Geel           *         *(Rood) >BLOK_VRIJ
            //  Groen   Groen          *         *           *

            // Ik neem meteen de seinState mee.
            // Als je b.v na dit stuk de sein state leest zou die
            // wel eens op wissel_rood kunnen staan en dat is niet
            // gewenst om te copieren
            seinState = SEIN_GROEN;       // default case
            if ((pKop->pBlok->RegelaarNummer != Regelaar) ||
                    ((pKop->pBlok->Bovenleiding == 0) &&
                     pInfo->RegelArray[Regelaar].ELoc))
            {
                prepareSein (pBlok, SEIN_ROOD);
                seinState = SEIN_ROOD;
            }
            else
            {
                if ((pBlokVolg->pBlok->State > BLOK_VRIJ) ||
                        ((pBlokVolg->pBlok->Bovenleiding == 0) &&
                         pInfo->RegelArray[Regelaar].ELoc))
                {
                    seinState = SEIN_GEEL;
                    prepareSein (pBlok, SEIN_GEEL);
                    prepareSein (pKop, SEIN_ROOD);
                }
                else
                {
                    if ((pBlokVolgVolg->pBlok->State > BLOK_VRIJ) ||
                            ((pBlokVolgVolg->pBlok->Bovenleiding == 0) &&
                             pInfo->RegelArray[Regelaar].ELoc))
                    {
                        // pBlok is nu groen maar dat wordt die vanzelf
                        // als ik niets doe
                        prepareSein (pKop, SEIN_GEEL);
                        prepareSein (pBlokVolg, SEIN_ROOD);
                    }
                    // Geen else case want ik hoef geen blokken groen te zetten
                }
            }

            // Nu gaan we de achterliggende blokken behandelen
            // trein trein  vrij  vrij vrij
            // XXX   XXX   ROOD  GEEL GROEN

            // Denk eraan dat we nu tegen de richting in gaan!
            do
            {
                if (State == BLOK_VOORUIT)
                {
                    pBlok = pBlok->pVorigBlok;
                }
                else
                {
                    pBlok = pBlok->pVolgendBlok;
                }
                // als dit blok bij ons behoord copieer
                // dan de sien state erin


                if (pBlok->pBlok->RegelaarNummer == Regelaar)
                {
                    prepareSein (pBlok, seinState);
                }
            }
            while ((pBlok->pBlok->RegelaarNummer == Regelaar)
                   && (pBlok != pInfo->RegelArray[Regelaar].pKopBlok));
            // ik test in de while ook of we niet ons eigen kopBlok weer
            // tegen komen. Trein komt zichzelf tegen dan kan ik maar beter stoppen

            if (pBlok->pBlok->State == BLOK_VRIJ)
            {
                // Als het achterliggende blok vrij is dan mag ik
                // hem rood maken.
                prepareSein (pBlok, SEIN_ROOD);
                if (State == BLOK_VOORUIT)
                {
                    pBlok = pBlok->pVorigBlok;
                }
                else
                {
                    pBlok = pBlok->pVolgendBlok;
                }
                if (pBlok->pBlok->State == BLOK_VRIJ)
                {
                    // Als het volgde achterliggende blok vrij is dan mag
                    // ik hem geel maken
                    prepareSein (pBlok, SEIN_GEEL);
                    // en groen maken is niet nodig
                }
            }
        }
    }

    // update de seininformatie naar de baan
    for (blok = 0; blok < MAX_NOBLOKS; blok++)
    {
        pBlok = &pInfo->BlokPointer[blok];
        if (pBlok->BlokIONummer == blok)
        {
            if (pBlok->pBlok->blokSein != GEEN_SEIN)
            {
                // blok is gebruikt en heeft een seinpaal
                zetSein (pBlok, &array, pBlok->pBlok->seinState);
            }
        }

        // Voor de volgende ronde even default groen maken
        if (pBlok->pBlok->seinState <= SEIN_ROOD)
            pBlok->pBlok->seinState = SEIN_GROEN;
    }


}

int BaanWT::BaanTestBelegCheckblok (int regelaarNummer)
{
    BlokPointer_t *pBlok, *pVolgBlok;
    int Snelheid, State;

    pBlok = pInfo->RegelArray[regelaarNummer].pKopBlok;

    State = pBlok->pBlok->State;
    if (State == BLOK_VOORUIT)
    {
        pVolgBlok = pBlok->pVolgendBlok;
    }
    else
    {
        pVolgBlok = pBlok->pVorigBlok;
    }
    Snelheid = pBlok->pBlok->Snelheid;

    if ((pVolgBlok->pBlok->State == BLOK_VRIJ) &&
            (pInfo->RegelArray[regelaarNummer].getGewensteSnelheid () != 0))
    {
        /* Het kopblok is nog vrij beleg hem snel */
        // belegging alleen als er een snelheid instelling is
        pVolgBlok->pBlok->RegelaarNummer = regelaarNummer;
        if (State == BLOK_VOORUIT)
        {
            pVolgBlok->pBlok->State = BLOK_VOORUITCHECK;
            pVolgBlok->pBlok->hardwareState = BLOK_VRIJ;  // de detectie kan nu de richting doen
        }
        else
        {
            pVolgBlok->pBlok->State = BLOK_ACHTERUITCHECK;
            pVolgBlok->pBlok->hardwareState = BLOK_VRIJ;  // de detectie kan nu de richting doen
        }
        pVolgBlok->pBlok->Snelheid = Snelheid;
        mBaanMessage.Post (WM_BLOK_DISPLAY, pVolgBlok->pBlok - pInfo->Blok,
                           regelaarNummer, BLOK_KOPBLOK);
        return 1;

    }
    if (((pVolgBlok->pBlok->State == BLOK_VOORUITCHECK) ||
         (pVolgBlok->pBlok->State == BLOK_ACHTERUITCHECK)) &&
            (pVolgBlok->pBlok->RegelaarNummer == regelaarNummer) &&
            (Snelheid == 0) &&
            (pInfo->RegelArray[regelaarNummer].getGewensteSnelheid () == 0))
    {
        // het is ons check blok en we hebben geen snelheid meer
        // geef hem dan weer vrij
        GeefBlokVrij (pVolgBlok);
    }
    return 0;
}

/*
** Deze functie kijkt of het volgende blok vrij is en regelt
** de snelheid.
**/
void BaanWT::BaanSnelheidControl (void)
{
    int State, Snelheid, Richting, Regelaar;
    BlokPointer_t *pBlok, *pKop;

    for (Regelaar = 0; Regelaar < MAX_AANTAL_REGELAARS; Regelaar++)
    {
        if (pInfo->RegelArray[Regelaar].Gebruikt == 0)
        {
            continue;
        }

        // regelaar is gebruikt

        // kijk of het kopblok al ingebruik is
        BaanTestBelegCheckblok (Regelaar);

        pBlok = pInfo->RegelArray[Regelaar].pKopBlok;
        if (pBlok->pBlok->specialProcessing)
            pBlok->pBlok->specialProcessing();

        State = pBlok->pBlok->State;
        if (State == BLOK_VOORUIT)
            pKop = pBlok->pVolgendBlok;
        else
            pKop = pBlok->pVorigBlok;

        // Geef de hardware spanning door zodat de
        // regelaar zijn werk kan doen
        Snelheid = pBlok->pBlok->hardwareReturnWaarde - pBlok->pBlok->nulWaarde;
        if (State == BLOK_VOORUIT)
            // bij vooruit verwacht ik een negative spanning
            Snelheid = -Snelheid;
        // als de spanning nog steeds negatief is dan klip ik hem naar
        // naar 0. Dit gebeurd bij een motor die spanning heeft maar
        // nog niet wil lopen (negative reactie van de spoel)
        if (Snelheid < 0)
            Snelheid = 0;
        if (Snelheid > 63)
            Snelheid = 63;
        pInfo->RegelArray[Regelaar].GeefProgress (Snelheid);

        Richting = pInfo->RegelArray[Regelaar].Richting;
        if (Richting)
            Richting = BLOK_ACHTERUIT;
        else
            Richting = BLOK_VOORUIT;
        if (pBlok->pBlok->State != Richting)
        {
            /* Richting verandering eerst afremmen */
            if (pBlok->pBlok->Snelheid != 0)
            {
                int s;
                /* DO stop actie */
                s = pInfo->RegelArray[Regelaar].GeefStopSnelheid ();
                SetSnelheid (Regelaar, s);
            }
            else
            {
                BlokPointer_t *pTBlok;

                // Ik stuur geen extra richting verandering berichten
                // dit wordt door het check blok zelf gedaan en de
                // rest volgt tijdens SetSnelheid

                /* Snelheid is nu 0 dus verwissel rijrichting */
                if (pKop->pBlok->RegelaarNummer == Regelaar)
                {
                    /* Het kopblok behoord bij ons dus geef hem vrij */
                    pKop->pBlok->State = BLOK_VRIJ;
                    pKop->pBlok->Snelheid = 0;
                    pKop->pBlok->RegelaarNummer = -1;
                    mBaanMessage.Post (WM_BLOK_DISPLAY, pKop->pBlok - pInfo->Blok,
                                       Regelaar, BLOK_CLEAR);
                }
                pTBlok = pBlok;
                do
                {
                    pTBlok->pBlok->State = Richting;
                    /* Snelheid is al 0 dus zet ik hem niet */
                    if (Richting == BLOK_VOORUIT)
                    {
                        pTBlok = pTBlok->pVolgendBlok;
                    }
                    else
                    {
                        pTBlok = pTBlok->pVorigBlok;
                    }
                    if (pTBlok->pBlok->RegelaarNummer == Regelaar)
                    {
                        /* neem de kop positie over in de regelaar */
                        pInfo->RegelArray[Regelaar].pKopBlok = pTBlok;
                    }
                }
                while ((pTBlok->pBlok->RegelaarNummer == Regelaar) && (pBlok != pTBlok)); // ook protectie als we rond zijn
                /*
               ** De Kop is nu verwisselt ga eruit en bij de volgende
               ** slag word er naar het kopblok gekeken
               **/
                pBlok = pInfo->RegelArray[Regelaar].pKopBlok;
            }
        }
        else
        {
            BlokPointer_t *pTBlok;
            int Lengte;

            // ik bepaal eerst de lengte van de vrije blokken voor me
            pTBlok = pBlok;
            Lengte = 0;
            if (Richting == BLOK_VOORUIT)
                pTBlok = pTBlok->pVolgendBlok;
            else
                pTBlok = pTBlok->pVorigBlok;
            /* check ook even de maximum snelheid van het check blok */
            if (pTBlok->pBlok->Stop == Regelaar)
            {
                // stop conditie op het check blok
                Lengte = pTBlok->Lengte;
            }
            else
            {
                // geen stop conditie dus een normale lengte check
                while (((pTBlok->pBlok->State == BLOK_VRIJ) || (pTBlok->pBlok->RegelaarNummer == Regelaar)) && ((pTBlok->pBlok->Bovenleiding == 1) || (pInfo->RegelArray[Regelaar].ELoc == 0)) && (Lengte < 300))   /* kijk of we meer dan een 3 meter krijgen */
                {
                    if (pTBlok->Lengte >= MIN_LENGTE)
                    {
                        // voor zeer kleine blokken geen lengte want daar kan je toch
                        // niet in stoppen
                        Lengte += pTBlok->Lengte;
                    }
                    if (Richting == BLOK_VOORUIT)
                        pTBlok = pTBlok->pVolgendBlok;
                    else
                        pTBlok = pTBlok->pVorigBlok;
                }
            }
            /* check de maximum snelheid in de staart van de trein */
            // richting is onverandered
            // Nu kijken we eerst voor een eloc of het kopblok bovenleiding heeft
            // en ook of de lengte niet te klein is
            if (((pKop->pBlok->Bovenleiding == 0)
                 && (pInfo->RegelArray[Regelaar].ELoc)) ||
                    (Lengte < MIN_LENGTE))
            {
                int s;
                /* DO stop actie */
                s = pInfo->RegelArray[Regelaar].GeefStopSnelheid ();
                SetSnelheid (Regelaar, s);
            }
            else
            {
                //
                // check voor stop actie of een normale snelheids check
                //
                if ((pKop->pBlok->RegelaarNummer != Regelaar) ||
                        (pBlok->pBlok->Stop == Regelaar))
                {
                    int s;
                    /* DO stop actie */
                    // In verband met eventuele kortsluiting in de verlichting blokken
                    // doen we altijd alle blokken beschrijven.
                    s = pInfo->RegelArray[Regelaar].GeefStopSnelheid ();
                    SetSnelheid (Regelaar, s);
                }
                else
                {

                    /* Kopblok is van ons doe normale snelheid check */
                    int s;
                    int MaxSnelheid;


                    /* Check de maximum snelheid */
                    MaxSnelheid = pBlok->pBlok->MaxSnelheid;


                    /* Controleer of we binnen de gestelde lengte kunnen stopppen */
                    pTBlok = pBlok;
                    if (Richting == BLOK_VOORUIT)
                        pTBlok = pTBlok->pVolgendBlok;
                    else
                        pTBlok = pTBlok->pVorigBlok;
                    /* check ook even de maximum snelheid van het check blok */
                    if (pTBlok->pBlok->MaxSnelheid < MaxSnelheid)
                    {
                        MaxSnelheid = pTBlok->pBlok->MaxSnelheid;
                    }
                    /* check de maximum snelheid in de staart van de trein */
                    pTBlok = pBlok;
                    do
                    {
                        if (pTBlok->pBlok->MaxSnelheid < MaxSnelheid)
                        {
                            MaxSnelheid = pTBlok->pBlok->MaxSnelheid;
                        }
                        /* let op we gaan achteruit de staart in */
                        if (Richting == BLOK_VOORUIT)
                            pTBlok = pTBlok->pVorigBlok;
                        else
                            pTBlok = pTBlok->pVolgendBlok;
                    }
                    while ((pTBlok->pBlok->RegelaarNummer == Regelaar) &&
                           (pTBlok != pBlok));


                    /* Reken de maximum snelheid om in ticken van deze regelaar */
                    // note topsnelheid is gedefineerd als 63/topsnelheid * 16384
                    MaxSnelheid =
                            (pInfo->RegelArray[Regelaar].TopSnelheid *
                             MaxSnelheid + 8192) >> 14;
                    Snelheid =
                            pInfo->RegelArray[Regelaar].
                            GeefRemSnelheid (Lengte);
                    if (Snelheid < MaxSnelheid)
                    {
                        // We kunnen niet snel genoeg stoppen gegeven de maximum snelheid
                        // Dus neem de gegeven remweg snelheid
                        MaxSnelheid = Snelheid;
                    }
                    s =
                            pInfo->RegelArray[Regelaar].
                            GeefSnelheid (MaxSnelheid);
                    SetSnelheid (Regelaar, s);
                }
            }
        }
    }
}


void BaanWT::BaanBehandelIO (hardwareCom_t * array)
{
    int i;
    int aantal;

    // comprimeer de array to een maximum van MAX_ACTIVE_REGELAARS
    for (i = 0, aantal = 0; i < MAX_AANTAL_REGELAARS; i++)
    {
        hardwareArray_t *parray;

        parray = &array->array[i];
        if (parray->adres != -1)
        {
            if (MAX_ACTIVE_REGELAARS == aantal)
            {
                // we zijn aan het maximum gekomen dus de rest weg halen
                parray->adres = -1;
                printf ("Gooi weg\n");
            }
            else
            {
                // copieren alleen gebruikte regelaars
                if (aantal != i)
                {
                    array->array[aantal++] = *parray;
                    parray->adres = -1;
                }
                else
                {
                    aantal++;
                }
            }
        }
    }
    // vul nu de rest op
    i = aantal;
    array->aantal = aantal;
    for (; i < MAX_MICRO_MEM; i++)
    {
        // een ongebruikte entry check eerst de hoge prioriteit
        if (pInfo->hardwareHoog.krijgItem (array->array[i]))
        {
            // die is leeg dus nu de lage prioriteit
            if (pInfo->hardwareLaag.krijgItem (array->array[i]))
            {
                // die is ook leeg dus einde
                return;
            }
        }
        array->aantal += 1;
    }
}


// Deze functie gaat na voor alle gebruikte regelaars of er
// een verandering is. B.V. stop detectie of snelheid verandering
void BaanWT::BaanDetection (hardwareCom_t * array)
{
    int i;
    // TODO kombineer deze met snelheid control in 1
    // dan loopt alles per 100ms en tussen de twee aansturing kan ik dan andere
    // dingen doen?
    // Ik vrees voor extra delay. Het levert te weinig op. Verwerpen dus?

    for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
    {
        hardwareArray_t *parray;

        parray = &array->array[i];
        if (pInfo->RegelArray[i].Gebruikt != 0)
        {
            int Detect;
            int Snelheid, State;
            BlokPointer_t *pBlok, *pVolgBlok;

            pBlok = pInfo->RegelArray[i].pKopBlok;

            // Check voor een stop conditie
            if (pInfo->Stop)
            {
                // Reset de regelaars
                pInfo->RegelArray[i].RegelaarStop ();
            }

            /* Doe tijd registratie */
            if (pBlok->pBlok->Snelheid != 0)
                pInfo->RegelArray[i].TotaalTicken += 1;

            State = pBlok->pBlok->State;
            if (State == BLOK_VOORUIT)
            {
                pVolgBlok = pBlok->pVolgendBlok;
            }
            else
            {
                pVolgBlok = pBlok->pVorigBlok;
            }

            if (pVolgBlok->pBlok->RegelaarNummer == i)
            {
                int Spanning;

                if (pVolgBlok->pBlok->specialProcessing)
                    pVolgBlok->pBlok->specialProcessing();

                // Alleen als het volgende blok een dezelfde regelaar is
                // Dan mogen we kijken naar de detector
                Detect = 0;
                Snelheid = pVolgBlok->pBlok->Snelheid;
                if (Snelheid != 0)
                {
                    Spanning = pVolgBlok->pBlok->hardwareReturnWaarde;
                    if (Spanning != 0)
                    {
                        Spanning = Spanning - pVolgBlok->pBlok->nulWaarde;
                        //                    printf("%d\n",Spanning);
                        if ((Spanning < -DETECT_DREMPEL)
                                || (Spanning > DETECT_DREMPEL))
                        {
                            Detect = 1;
                        }
                    }
                }

                if (Detect)
                {
                    /*
                   ** Trein is in een nieuw blok gereden
                   **/
                    pVolgBlok->pBlok->DetectorCount += 1;
                    if (pVolgBlok->pBlok->DetectorCount == AANTAL_DETECTIES)
                    {
                        clock_t verschil;
                        /* Tel de lengte bij het totaal */
                        pInfo->RegelArray[i].TotaalAfstand +=
                                pBlok->Lengte;

                        // Bereken de snelheid in Km/h gemiddeld over dit blok
                        pInfo->RegelArray[i].blokLengte += pBlok->Lengte;
                        verschil =
                                baanTimeGetInms () - pInfo->RegelArray[i].tijd;
                        if (verschil > 2000)
                        {
                            float tijd;
                            int echteSnelheid;

                            tijd = (float) (verschil) / 1000.0f;
                            echteSnelheid =
                                    (int) ((float) pInfo->RegelArray[i].
                                           blokLengte / tijd * 87.0 * 3.6 /
                                           100.0 + 0.5);
                            pInfo->RegelArray[i].
                                    EchteSnelheid (echteSnelheid);
                            pInfo->RegelArray[i].tijd =
                                    baanTimeGetInms ();
                            pInfo->RegelArray[i].blokLengte = 0;
                        }
                        pInfo->RegelArray[i].pKopBlok = pVolgBlok;
                        pVolgBlok->pBlok->State = State;
                        pVolgBlok->pBlok->hardwareState = State;
                        pVolgBlok->pBlok->RegelaarNummer = i;
                        // Zet een dot op het nieuwe regelaar blok
                        mBaanMessage.Post (WM_BLOK_DISPLAY,
                                           pVolgBlok->pBlok - pInfo->Blok, i,
                                           BLOK_BELEG);
                        mBaanMessage.Post (WM_BLOK_EVENT, i,
                                           pInfo->RegelArray[i].
                                           pKopBlok->pBlok->hardwareAdres, -1);
                        /*
                       ** Check of we een blok moeten vrij geven
                       **/
                        if (pInfo->RegelArray[i].laatsteWagonCheck == 0)
                        {
                            // De laatste wagon heeft geen condensator dus
                            // geven we vrij als er een detectie is.
                            BaanCheckLengte (i, 0);
                        }
                        pInfo->RegelArray[i].ResetIgnoreStop ();

                        // kijk of het volgende blok vrij is.
                        if (State == BLOK_VOORUIT)
                        {
                            pVolgBlok = pVolgBlok->pVolgendBlok;
                        }
                        else
                        {
                            pVolgBlok = pVolgBlok->pVorigBlok;
                        }
                        BaanTestBelegCheckblok (i);
                        // pas op pVolgBlok wijst soms naar een bezet blok van een
                        // andere regelaar!

                    }
                }
                else
                {
                    pVolgBlok->pBlok->DetectorCount = 0;
                }

                State = pVolgBlok->pBlok->State;
                parray->blokIO = HW_BLOK;

                if (pVolgBlok->pBlok->RegelaarNummer == i)
                {
                    // ik test weer op regelaar nummer want bij een detectie
                    // is pVolgBlok verandered
                    if (State != pVolgBlok->pBlok->hardwareState)
                    {
                        // zend eerst een vooruit terug commando
                        if (State == BLOK_VOORUITCHECK)
                        {
                            parray->data = pInfo->Vooruit;
                        }
                        else
                        {
                            parray->data = pInfo->Achteruit;
                        }
                        pVolgBlok->pBlok->hardwareState = State;
                        pVolgBlok->pBlok->hardwareReturnWaarde =
                                pVolgBlok->pBlok->nulWaarde;
                        // het voor/terug wisseling geeft blokOn signaal terug
                        // en dat gebruiken we nog niet. Als ik die waarde save
                        // kan dit tot valse detecties leiden.
                        parray->returnGewenst = 0;
                        parray->adres = pVolgBlok->pBlok->hardwareAdres;
                        parray->nummer = parray->adres;
                    }
                    else
                    {
                        if ((pVolgBlok->pBlok->Snelheid != 0) ||
                                (pBlok->pBlok->Snelheid != 0))
                        {
                            // alleen als er gereden wordt stuur ik het check blok aan
                            int decimatie;

                            // bij de normale snelheid setting een return waarde
                            // ik decimeer het aantal keren dat ik het check blok check
                            // Dit is afhankelijk van de lengte van het check blok
                            decimatie = 2;
                            if (pVolgBlok->Lengte < 50)
                            {
                                decimatie = 1;
                            }
                            pInfo->RegelArray[i].decimatie += 1;
                            if (pInfo->RegelArray[i].decimatie >=
                                    decimatie)
                            {
                                pInfo->RegelArray[i].decimatie = 0;

                                parray->data = pBlok->pBlok->Snelheid;    // snelheid van kopblok
                                parray->returnGewenst = 1;
                                parray->adres = pVolgBlok->pBlok->hardwareAdres;
                                parray->nummer = parray->adres;
                                // snelheid wordt nu overgenomen dus zet hem
                                pVolgBlok->pBlok->Snelheid =
                                        pBlok->pBlok->Snelheid;
                            }
                            // else is parray->adres = -1 en dan wordt die niet gebruikt.
                            // De aansturing maak parray->adres = -1 als die uitgezonden is
                        }       // end if (pVolgBlok->pBlok->Snelheid != 0)
                    }
                }               // end if (pVolgBlok->pBlok->RegelaarNummer == i)
            }                   // end if (pVolgBlok->pBlok->RegelaarNummer == i)
        }                       // end if (pInfo->RegelArray[i].Gebruikt != 0)
    }                           // end for(i=0;i<MAX_AANTAL_REGELAARS;i++)
}



void BaanWT::InitWorkThread (void)
{
    int i;

    /* Initialize alles of 0 */
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        pInfo->BlokPointer[i].pBlok = &pInfo->Blok[i];
        pInfo->BlokPointer[i].pVolgendBlok = &pInfo->EindBlokPointer;
        pInfo->BlokPointer[i].pVorigBlok = &pInfo->EindBlokPointer;
        pInfo->BlokPointer[i].Lengte = 1;
        pInfo->BlokPointer[i].BlokIONummer = -1;  // ongebruikt blok
        pInfo->BlokPointer[i].BlokType = BAAN_BLOK;
        pInfo->Blok[i].State = BLOK_VRIJ;
        pInfo->Blok[i].Snelheid = 0;
        pInfo->Blok[i].MaxSnelheid = MAX_SNELHEID;
        pInfo->Blok[i].blokSein = GEEN_SEIN;
        pInfo->Blok[i].seinState = SEIN_GROEN;
        pInfo->Blok[i].hardwareSeinState = SEIN_GROEN;
        pInfo->Blok[i].Bovenleiding = 0;
        pInfo->Blok[i].Stop = -1;
        pInfo->Blok[i].RegelaarNummer = -1;
        pInfo->Blok[i].DetectorCount = 0;
        pInfo->Blok[i].hardwareState = BLOK_VRIJ;
        pInfo->Blok[i].hardwareAdres = i; // blok nummer is gelijk aan adres
        pInfo->Blok[i].hardwareReturnWaarde = 0x7f;
        pInfo->Blok[i].richtingVoorkeur = richtingBeiden;

        pInfo->Blok[i].XCoord = -1;       // illegale waarde zodat ik kan zien of die gebruikt wordt
        pInfo->Blok[i].YCoord = -1;

        pInfo->Blok[i].blokBelegt = 0;
        pInfo->Blok[i].blokBelegtRegelaar = -1;

    }

    /* Initializeer het eindblok */
    pInfo->EindBlokPointer.pBlok = &pInfo->EindBlok;
    pInfo->EindBlokPointer.pVolgendBlok = &pInfo->EindBlokPointer;;
    pInfo->EindBlokPointer.pVorigBlok = &pInfo->EindBlokPointer;;
    pInfo->EindBlokPointer.Lengte = 1;
    pInfo->EindBlokPointer.BlokIONummer = -1;
    pInfo->EindBlokPointer.BlokType = STOP_BLOK;
    pInfo->EindBlok.State = BLOK_STOP;
    pInfo->EindBlok.Snelheid = 0;
    pInfo->EindBlok.MaxSnelheid = MAX_SNELHEID;
    pInfo->EindBlok.blokSein = GEEN_SEIN;
    pInfo->EindBlok.seinState = SEIN_GROEN;
    pInfo->EindBlok.hardwareSeinState = SEIN_GROEN;
    pInfo->EindBlok.Bovenleiding = 1;
    pInfo->EindBlok.Stop = 0;
    pInfo->EindBlok.RegelaarNummer = -2;
    pInfo->EindBlok.XCoord = 2;   /* is used for reset position */
    pInfo->EindBlok.YCoord = 2;
    pInfo->EindBlok.richtingVoorkeur = richtingBeiden;

    pInfo->EindBlok.DetectorCount = 0;
    pInfo->noodStop = 0;

    pInfo->Simulatie = 0;

    pInfo->idleBlokCounter = 1;

    initSimulatieArray ();
    for (i = 0; i < MAX_MICRO_MEM; i++)
    {
        pInfo->centraleMicroMem[2 * i] = 0;
        pInfo->centraleMicroMem[2 * i + 1] = 0;
    }

    for (i = 0; i < MAX_MICRO_MEM_ALLOCATION; i++)
    {
        pInfo->hardwareDetection.array[i].blokIO = HW_BLOK;
        pInfo->hardwareDetection.array[i].adres = -1;
        pInfo->hardwareDetection.array[i].nummer = -1;
        pInfo->hardwareDetection.array[i].data = 0;
        pInfo->hardwareDetection.array[i].returnGewenst = 0;

        pInfo->hardwareSnelheid.array[i].blokIO = HW_BLOK;
        pInfo->hardwareSnelheid.array[i].adres = -1;
        pInfo->hardwareSnelheid.array[i].nummer = -1;
        pInfo->hardwareSnelheid.array[i].data = 0;
        pInfo->hardwareSnelheid.array[i].returnGewenst = 0;
    }
    pInfo->hardwareDetection.aantal = 0;
    pInfo->hardwareSnelheid.aantal = 0;
    pInfo->StopExecution = 0;
    pInfo->RegelaarAanvraag.Aanvraag = 0;
    pInfo->RegelaarAanvraag.NieuweRegelaar = 0;
    pInfo->RegelaarAanvraag.VerwijderRegelaar = 0;
    pInfo->GeefVrijAanvraag.GeefVrijAanvraag = 0;

    pInfo->vorigeStopStatus = 1;
    pInfo->Stop = 0;

    pInfo->debugAantalItemsX = 0;
    pInfo->debugAantalItemsY = 0;
    pInfo->SleepTime = CYCLETIME;

    pInfo->tickTimer = 0;
    for (i = 0; i < NO_OF_TIMERS; i++)
    {
        // ik neem aan dat er geen wrap arround ontstaat
        // het programma gebruikt een 100ms tick time
        // dus moet het programma 6 jaar onafgebroken aanstaan
        // voordat een overflow ontstaat
        pInfo->timerArray[i].ticks = -1;
    }




    pInfo->RegelaarAanvraag.semWacht = baanSemaphoreCreate (0);
    pInfo->semCriticalSection = baanSemaphoreCreate (1);
    pInfo->semStarted = baanSemaphoreCreate (0);


}

// Controlling function for the worker thread.
void BaanWT::BaanWorkerThreadProc (void)
{
    int Count = 0, Total = 0;
    int Difference;
    int err;

#ifdef DEBUG_TIME
    int minTime = 159876;
    int maxTime = 0;
#endif

#ifndef _WIN32
    {
        struct sched_param sp;
        int priority;
        priority = sched_get_priority_max (SCHED_FIFO);
        sp.sched_priority = priority;
        sched_setscheduler (0, SCHED_FIFO, &sp);

        printf ("Priority %d\n", sched_getscheduler (0));

    }
#endif

    StartHardware ();
    {
        // eerste de nulWaarde van elk blok bepalen door
        // alle blokken 0 te schrijven en de returnWaarde
        // van deze schrijf actie op te halen
        int arrayCount = 0;
        int i;
        hardwareCom_t *peerst, *pvorig, *ptemp;

        peerst = &pInfo->hardwareSnelheid;
        pvorig = &pInfo->hardwareDetection;

        for (i = 0; i < MAX_NOBLOKS; i++)
        {
            if (pInfo->BlokPointer[i].BlokIONummer != -1)
            {
                // blok wordt gebruikt
                hardwareArray_t *parray;

                parray = &peerst->array[arrayCount++];
                parray->adres = i;
                parray->nummer = i;
                parray->data = 0;
                parray->returnGewenst = 1;
                parray->blokIO = HW_BLOK;
                if (MAX_MICRO_MEM == arrayCount)
                {
                    peerst->aantal = arrayCount;
                    if ((err = BaanAansturing (peerst, pvorig)))
                    {
                        // jammer het message komt nog niet door
                        int x;
                        if (2 == err)
                        {
                            char buffer[1000];
                            sprintf (buffer,
                                     "Een of meerder blokken gaf geen antwoord");
                            for (x = 0; x < MAX_MICRO_MEM; x++)
                            {
                                sprintf (buffer, "%s%5d", buffer,
                                         pvorig->array[x].nummer);
                            }
                            mMessage.message(buffer);
                        }
                        else
                        {
                            mMessage.message(str(boost::format("Algemene communicatie fout %d") %
                                                 err));
                        }
                        ResetHardware ();
                    }
                    //                ericSleep (CYCLETIME);
                    arrayCount = 0; // reset voor de volgende set
                    // swap de communicatie array
                    ptemp = peerst;
                    peerst = pvorig;
                    pvorig = ptemp;
                }                 // if arrayCount
            }                     // if BlokIONummer
        }                         // for i

        if (arrayCount)
        {
            // er moet nog wat verzonden worden
            peerst->aantal = arrayCount;
            if ((err = BaanAansturing (peerst, pvorig)))
            {
                ResetHardware ();

            }
            ptemp = peerst;
            peerst = pvorig;
            pvorig = ptemp;
            //        ericSleep (CYCLETIME);
        }
        // nu de resultaten van de laatse transfer ophalen
        peerst->aantal = 0;
        if ((err = BaanAansturing (peerst, pvorig)))
        {
            ResetHardware ();
        }
        //    ericSleep (CYCLETIME);

        // alle blokken zijn nu met 0 geinitializeerd lees nu de return waardes
        for (i = 0; i < MAX_NOBLOKS; i++)
        {
            if (pInfo->BlokPointer[i].BlokIONummer != -1)
            {
                pInfo->Blok[i].nulWaarde = pInfo->Blok[i].hardwareReturnWaarde;
                //          printf ("blok %d ret %d\n", pInfo->BlokPointer[i].BlokIONummer,
                //                  pInfo->Blok[i].hardwareReturnWaarde);

            }
        }

    }
    baanSemaphoreUp (pInfo->semStarted);

    while (pInfo->StopExecution == 0)
    {
        int Time;
        int i;

        baanSemaphoreDown (pInfo->semCriticalSection);
        mTd.tdStart (tdwt);
        // registeer de begin tijd
        Time = baanTimeGetInms ();

        /*
       ** Begin van het processen
       **/

        /* Eerst kijken we of er een request is voor een
       ** nieuwe regelaar
       **/
        if (pInfo->RegelaarAanvraag.Aanvraag == 1)
        {
            RegelAanvraag ();
        }

        // check of er program timers verlopen zijn
        for (i = 0; i < NO_OF_TIMERS; i++)
        {
            // ik neem aan dat er geen wrap arround ontstaat
            // het programma gebruikt een 100ms tick time
            // dus moet het programma 6 jaar onafgebroken aanstaan
            // voordat een overflow ontstaat
            if (pInfo->timerArray[i].ticks > 0)
            {
                // deze is gebruikt
                // kijk of die due is
                if (pInfo->tickTimer >= pInfo->timerArray[i].tickTime)
                {
                    int eventType;
                    int event;
                    int regelaar;

                    // store deze lokale variabelen want
                    // als ik de timer vrij geef dan kan die
                    // gealloceerd worden en zou de info
                    // al overschreven kunne worden
                    eventType = pInfo->timerArray[i].eventType;
                    event = pInfo->timerArray[i].event;
                    regelaar = pInfo->timerArray[i].regelaar;

                    pInfo->timerArray[i].tickTime =
                            pInfo->tickTimer + pInfo->timerArray[i].ticks;
                    if (pInfo->timerArray[i].flag == TIMER_EENMALIG)
                    {
                        // verwijder de timer
                        pInfo->timerArray[i].ticks = -1;
                    }
                    // zend event
                    if (eventType == TIMER)
                    {
                        mBaanMessage.Post (WM_TIMER_VERLOPEN,
                                           regelaar, event, -1);
                    }
                    else
                    {
                        mBaanMessage.Post (WM_INTERNALTIMER_VERLOPEN,
                                           regelaar, event, -1);
                    }
                }
            }
        }

        //
        // Check of een van de spoelen een verandering heeft
        //
        mWissels.TestIOTimers ();

        if (pInfo->GeefVrijAanvraag.GeefVrijAanvraag == 1)
        {
            BaanGeefVrij (pInfo->GeefVrijAanvraag.BlokNummer);
            pInfo->GeefVrijAanvraag.GeefVrijAanvraag = 0;
        }

        /*
       ** Controleer of een trein in een nieuw baanvak is
       **/
        BaanDetection (&pInfo->hardwareDetection);

        pInfo->hardwareDetection.aantal = MAX_AANTAL_REGELAARS;
        // Na de detectie is de baan structuur veranderd dus moeten de seinen
        // omgezet worden
        BaanSetSein ();

        if (pInfo->debugAantalItemsX != 0)
        {
            // progress bars van de hardware aansturing

            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               (pInfo->debugAantalItemsY << 16),
                               MAX_AANTAL_AANVRAGEN + (5 << 16), IMainWindowDrawing::Gray66);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               (pInfo->debugAantalItemsY << 16),
                               pInfo->hardwareHoog.aantalItems () + (5 << 16),
                               IMainWindowDrawing::Red);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               ((pInfo->debugAantalItemsY + 10) << 16),
                               MAX_AANTAL_AANVRAGEN + (5 << 16), IMainWindowDrawing::Gray66);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               ((pInfo->debugAantalItemsY + 10) << 16),
                               pInfo->hardwareLaag.aantalItems () + (5 << 16),
                               IMainWindowDrawing::Red);

        }


        BaanBehandelIO (&pInfo->hardwareDetection);

        //
        // We zijn klaar en berekenen de slaap tijd
        // Eerst slapen dan de berekening weg sturen dan weet ik
        // zeker dat we altijd de CYCLETIME aan tijd verspijkert hebben
        // tussen versturen en berekenen
        //
        Difference = CYCLETIME - (baanTimeGetInms () - Time);
        if (Difference < 0)
        {
            Difference = 0;
        }
        else
        {
            // Ongeveer normaal gedrag
            Total += Difference;
            Count += 1;
        }
        mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                           pInfo->debugAantalItemsX +
                           ((pInfo->debugAantalItemsY + 30) << 16),
                           50 + (5 << 16), IMainWindowDrawing::Gray66);
        mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                           pInfo->debugAantalItemsX +
                           ((pInfo->debugAantalItemsY + 30) << 16),
                           Difference + (5 << 16), IMainWindowDrawing::Gray66);

        mTd.tdStop (tdwt);
        baanSemaphoreUp (pInfo->semCriticalSection);

        //      ericSleep (pInfo->SleepTime);

        // omdat er een cycle vertaging is met het verkrijgen van het antwoord
        // krijg ik de status terug van de check blok actie bij
        // de kopblok aansturing en vica versa.
        if ((err = BaanAansturing (&pInfo->hardwareDetection, &pInfo->hardwareSnelheid)))    // dit doet ook meteen wissels en seinen
        {
            ResetHardware ();
            if (err == 1)
            {
                mMessage.message(str(boost::format("1 Timeout error %d") %
                                     Difference));
            }
            else
            {
                mMessage.message("Een of meerder controlers gaf geen antwoord");
            }
        }
        // Oke ik houd de aansturing buiten de CYCLETIME
        // dit om te voorkomen dat we minder dan CYCLETIME wachten tussen
        // de twee BaanAanstruring
#ifdef DEBUG_TIME
        {
            int verschil;
            verschil = baanTimeGetInms () - Time;
            if (verschil < minTime)
            {
                minTime = verschil;
            }
            if (verschil > maxTime)
            {
                maxTime = verschil;
            }
        }
#endif
        baanSemaphoreDown (pInfo->semCriticalSection);
        mTd.tdStart (tdwt);


        Time = baanTimeGetInms ();

        BaanSnelheidControl ();      // dit doet ook wissels en seinen

        if (pInfo->debugAantalItemsX != 0)
        {
            // display de status van de hardware
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               (pInfo->debugAantalItemsY << 16),
                               MAX_AANTAL_AANVRAGEN + (5 << 16), IMainWindowDrawing::Gray66);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               (pInfo->debugAantalItemsY << 16),
                               pInfo->hardwareHoog.aantalItems () + (5 << 16),
                               IMainWindowDrawing::Red);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               ((pInfo->debugAantalItemsY + 10) << 16),
                               MAX_AANTAL_AANVRAGEN + (5 << 16), IMainWindowDrawing::Gray66);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               ((pInfo->debugAantalItemsY + 10) << 16),
                               pInfo->hardwareLaag.aantalItems () + (5 << 16),
                               IMainWindowDrawing::Red);

        }
        BaanBehandelIO (&pInfo->hardwareSnelheid);
        if (pInfo->hardwareSnelheid.aantal == MAX_MICRO_MEM)
        {
            // voor debugging om een breakpoint te kunnen zetten
            pInfo->hardwareSnelheid.aantal = MAX_MICRO_MEM;
        }

        //
        // We zijn klaar en berekenen de slaap tijd
        // Eerst slapen dan de berekening weg sturen dan weet ik
        // zeker dat we altijd de CYCLETIME aan tijd verspijkert hebben
        // tussen versturen en berekenen
        //
        Difference = CYCLETIME - (baanTimeGetInms () - Time);
        if (Difference < 0)
        {
            Difference = 0;
        }
        else
        {
            // Ongeveer normaal gedrag
            Total += Difference;
            Count += 1;
        }
        if (Count >= LOOPTIJD_BEREKENING)
        {
            // TODO middelen over meerder meetingen?
            pInfo->SleepTime = (Total + (Count >> 1)) / Count;

            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               ((pInfo->debugAantalItemsY + 40) << 16),
                               50 + (5 << 16), IMainWindowDrawing::Gray66);
            mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                               pInfo->debugAantalItemsX +
                               ((pInfo->debugAantalItemsY + 40) << 16),
                               pInfo->SleepTime + (5 << 16), IMainWindowDrawing::Red);
            Count = 0;
            Total = 0;
        }
        mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                           pInfo->debugAantalItemsX +
                           ((pInfo->debugAantalItemsY + 30) << 16),
                           50 + (5 << 16), IMainWindowDrawing::Gray66);
        mBaanMessage.Post (WM_VIERKANT_DISPLAY,
                           pInfo->debugAantalItemsX +
                           ((pInfo->debugAantalItemsY + 30) << 16),
                           Difference + (5 << 16), IMainWindowDrawing::Red);
        mTd.tdStop (tdwt);

        baanSemaphoreUp (pInfo->semCriticalSection);
        //      ericSleep (pInfo->SleepTime);

        // omdat er een cycle vertaging is met het verkrijgen van het antwoord
        // krijg ik de status terug van de check blok actie bij
        // de kopblok aansturing en vica versa.
        if ((err = BaanAansturing (&pInfo->hardwareSnelheid, &pInfo->hardwareDetection)))    // dit doet ook meteen wissels en seinen
        {
            ResetHardware ();
            if (err == 1)
            {
                mMessage.message(str(boost::format("2 Timeout error %d") %
                                     Difference));
            }
            else
            {
                mMessage.message("Een of meerder controlers gaf geen antwoord");
            }
        }
#ifdef DEBUG_TIME
        {
            int verschil;
            verschil = baanTimeGetInms () - Time;
            if (verschil < minTime)
            {
                minTime = verschil;
            }
            if (verschil > maxTime)
            {
                maxTime = verschil;
            }
        }
#endif

        pInfo->tickTimer += 1;

    }

    StopHardware ();
    baanSemaphoreUp (pInfo->semWorkerThreadStopped);
}

BaanInfo::BaanInfo(BaanInfo_t * baanInfo)
{
    m_baanInfo = baanInfo;
}

BlokPointer_t * BaanInfo::GetBlokPointer(int blokNummer)
{
    if ((blokNummer>0) && (blokNummer < m_baanInfo->AantalBlokken))
        return &m_baanInfo->BlokPointer[blokNummer];
    else
        return NULL;
};

BaanZoekResult BaanInfo::NextBlok(BlokPointer_t *input, int richting)
{
    int stand1,stand2,stand3;
    stand1 = stand2 = stand3 = 0;
    int richt = richting - RICHTING_VOORUIT;
    BlokPointer_t * nextBlok = input->blokRicht[richt];
    if (nextBlok->BlokType == STOP_BLOK)
    {
        if (nextBlok == &m_baanInfo->EindBlokPointer)
        {
            return BaanZoekResult( 0.0f, 0, 0, 0, NULL);
        }
    }
    
    stand1 = 1;
    return BaanZoekResult(nextBlok->BlokIONummer,
                          stand1,stand2,stand3,nextBlok);
}

std::vector<int> & BaanInfo::GetBlokList(int wisselNummer, int knoopPuntNummer)
{
    return m_baanInfo->IOBits[wisselNummer].get()->routeKnoopPunt[knoopPuntNummer].blokList;
}
