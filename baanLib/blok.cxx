#include "blok.h"
#include "td.h"
#include "boost/format.hpp"


Blok::Blok(IMessage& msg, IMainWindowDrawing& mainWindowDrawing, ITd &td, BaanInfo_t *baanInfo):
    mMessage(msg),
    mMainWindowDrawing(mainWindowDrawing),
    mTd(td),
    mBaanInfo(baanInfo)
{
    lastVrij = 1;
    tdview = mTd.tdCreate("viewBlok");
    tdOnBlokDisplay = mTd.tdValueString ("OnBlokDisplay");
}

char * Blok::BlokNaam (char *string, BlokPointer_t * blok)
{
    if ((blok < &mBaanInfo->BlokPointer[MAX_NOBLOKS]) &&
            (blok > &mBaanInfo->BlokPointer[0]))
    {
        sprintf (string, "B%d", blok->BlokIONummer);
    }
    else
    {
        if (blok == &mBaanInfo->EindBlokPointer)
        {
            sprintf (string, "B-1");
        }
        else
        {
            for (size_t i=0;i<mBaanInfo->IOBits.size();i++)
            {
                auto pWissel = mBaanInfo->IOBits[i].get();
                for (size_t s=0; s<pWissel->StopBlokPointer.size();s++)
                {
                    if (blok == &pWissel->StopBlokPointer[s])
                    {
                        if (s==0)
                        {
                            sprintf (string, "W%d.%02d",
                                     pWissel->hardwareAdres,
                                     pWissel->hardwareBit);
                            return string;
                        }
                        else
                        {
                            sprintf (string, "w%d.%02d",
                                     pWissel->hardwareAdres,
                                     pWissel->hardwareBit);
                            return string;

                        }
                    }
                }
            }
            sprintf (string, "XXXX");
            return NULL;
            // wijst naar een wissel
        }
    }
    return string;
}


void Blok::BlokDisplay (int blokNummer, int regelaar, int actie)
{
    Blok_t *pBlok;
    std::string regelBitmapName;
    char string[20];

    mTd.tdStart (tdview);

    pBlok = &mBaanInfo->Blok[blokNummer];
    mTd.tdValue (tdOnBlokDisplay, (pBlok - mBaanInfo->Blok));


    if (regelaar != -1)
    {
        regelBitmapName = mBaanInfo->RegelArray[regelaar].bitmapName;
    }

    if ((actie==BLOK_CLEAR) && (mBaanInfo->editMode))
    {
        sprintf (string, "%c%d%c",
                 (mBaanInfo->Blok[blokNummer].richtingVoorkeur & richtingVooruit) ? '<' : ' ',
                 mBaanInfo->BlokPointer[blokNummer].BlokIONummer,
                 (mBaanInfo->Blok[blokNummer].richtingVoorkeur & richtingAchteruit) ? '>' : ' ');

        std::string blokText(string);
        BlokPointer_t *volgend;
        volgend = mBaanInfo->BlokPointer[blokNummer].pVolgendBlok;
        BlokNaam (string, volgend);
        blokText += string;

        mMainWindowDrawing.BlokClearEditModeDisplay(blokNummer == mBaanInfo->selectedBlok,  pBlok->XCoord, pBlok->YCoord, blokText);
    }
    else
    {

        mMainWindowDrawing.BlokDisplay(regelaar,actie, pBlok->XCoord, pBlok->YCoord, regelBitmapName, mBaanInfo->BlokPointer[blokNummer].BlokIONummer);
    }
    mTd.tdStop (tdview);

}

// newBlok zijn pVolgendBlok en pVorigBlok moeten
// al correct zijn!
//
// start
//               -----
//               |  -|-------|
//               |---|       |
//       |-------|-  |       |
//       |       -----       |
//                           
//     -----     -----     -----
//     |  -|---->|  -|---->|   |
//     |---|     |---|     |---|
//     |   |<----|-  |<----|-  |
//     -----     -----     -----
//
// stap 1 verwijderen oude referenties b.v. pvolgend
//
//               -----
//               |  -|-------|
//               |---|       |
//       |-------|-  |       |
//       |       -----       |
//                           
//     -----     -----     -----
//     |  -|---->| 0 |     |   |
//     |---|     |---|     |---|
//     |   |<----|-  |<----|-  |
//     -----     -----     -----
//
// stap 1 pvorig
//               -----
//               |  -|-------|
//               |---|       |
//       |-------|-  |       |
//       |       -----       |
//                           
//     -----     -----     -----
//     |  -|---->| 0 |     |   |
//     |---|     |---|     |---|
//     |   |     | 0 |<----|-  |
//     -----     -----     -----
//
// stap 2 aanbrengen pointers
//
//               -----
//           |-->|  -|-------|
//           |   |---|       |
//       |---+---|-  |<--|   |
//       |   |   -----   |   |
//           |           |   
//     ----- |   -----   | -----
//     |  -|--   | 0 |   | |   |
//     |---|     |---|   | |---|
//     |   |     | 0 |   --|-  |
//     -----     -----     -----
//

void Blok::BlokInsert (BlokPointer_t * newBlok)
{
    int richting, tegenRichting;

    for (richting = 0; richting < 2; richting++)
    {
        tegenRichting = (richting + 1) & 1;
        // stap 1
        if ((newBlok->blokRicht[richting] != &mBaanInfo->EindBlokPointer) &&
                (newBlok->blokRicht[richting] != NULL))
        {
            BlokPointer_t *temp;
            //  het nieuwe volgende blok
            temp = newBlok->blokRicht[richting];
            // van dit blok kijken of de pVorigBlok gezet is
            if ((temp->blokRicht[tegenRichting] != &mBaanInfo->EindBlokPointer) &&
                    (temp->blokRicht[tegenRichting] != NULL))
            {
                // nu de volgend van die pvorig null maken
                temp->blokRicht[tegenRichting]->blokRicht[richting] =
                        &mBaanInfo->EindBlokPointer;
            }
            // stap 2 nu de aansluiting maken
            temp->blokRicht[tegenRichting] = newBlok;
        }
    }
}

//
// BlokDelete
// Deze routine werkt alleen voor blokken niet voor 
// wissels (stopblokken)
//
// Van
//     -----     -----     -----
//     |  -|---->|  -|---->|   |
//     |---|     |---|     |---|
//     |   |<----|-  |<----|-  |
//     -----     -----     -----
//
// Naar
//     -----               -----
//     |  -|-------------->|   |
//     |---|               |---|
//     |   |<--------------|-  |
//     -----               -----
//               -----
//               | 0 | 
//               |---| 
//               | 0 | 
//               ----- 

void Blok::BlokDelete (BlokPointer_t * delBlok)
{
    int richting, tegenRichting;

    if ((delBlok > &mBaanInfo->BlokPointer[0]) &&
            (delBlok < &mBaanInfo->BlokPointer[MAX_NOBLOKS]))
    {
        for (richting = 0; richting < 2; richting++)
        {
            tegenRichting = (richting + 1) & 1;

            if (delBlok->blokRicht[richting] != &mBaanInfo->EindBlokPointer)
            {
                BlokPointer_t *temp;

                //  het nieuwe volgende blok
                temp = delBlok->blokRicht[richting];
                // van dit blok kijken of de pVorigBlok gezet is
                temp->blokRicht[tegenRichting] = &mBaanInfo->EindBlokPointer;
            }
        }
        // markeer ongebruikt
        delBlok->BlokIONummer = -1;
        delBlok->pVolgendBlok = &mBaanInfo->EindBlokPointer;
        delBlok->pVorigBlok = &mBaanInfo->EindBlokPointer;
    }
    else
    {
        mMessage.message
                (str(boost::format("blok %x niet deleted want het is geen baan blok maar iets van een wissel") %
                     delBlok));
    }
}

void Blok::BlokEndPointDelete (BlokPointer_t * delBlok, int richting)
{
    BlokPointer_t *blok;
    int tegenRichting;

    tegenRichting = (richting + 1) & 1;

    blok = delBlok->blokRicht[richting];
    blok->blokRicht[tegenRichting] = &mBaanInfo->EindBlokPointer;
    delBlok->blokRicht[richting] = &mBaanInfo->EindBlokPointer;
}

void Blok::BlokEndPointInsert (BlokPointer_t * insBlok, int richting)
{
    BlokPointer_t *blok;
    int tegenRichting;

    tegenRichting = (richting + 1) & 1;

    blok = insBlok->blokRicht[richting];
    if ((blok->blokRicht[tegenRichting] == &mBaanInfo->EindBlokPointer) ||
            (blok->blokRicht[tegenRichting] == NULL))
    {
    }
    else
    {
        // het blok waar we naartoe wijzen wijst naar een ander blok die maken we maar null
        BlokEndPointDelete (blok->blokRicht[tegenRichting], tegenRichting);
    }
    blok->blokRicht[tegenRichting] = insBlok;
    insBlok->blokRicht[richting] = blok;  // voor het geval dat blok nog naar onsezelf terug wees
}

void Blok::BlokPrint (BlokPointer_t * blok)
{
    char string[10];
    BlokNaam (string, blok);
    printf ("%s\n", string);
}



int Blok::BlokIsBlokNummer (int blokNummer)
{
    if ((blokNummer < 1) || (blokNummer >= MAX_NOBLOKS))
    {
        return 0;
    }
    return 1;
}


int Blok::BlokIsVrij (int blokNummer)
{
    int ret;
    if (BlokIsBlokNummer (blokNummer) == 0)
    {
        // blok niet goed
        return 0;
    }
    if (mBaanInfo->BlokPointer[blokNummer].BlokIONummer != -1)
    {
        ret = 0;
    }
    else
    {
        lastVrij = blokNummer;
        ret = 1;
    }
    return ret;
}

int Blok::BlokVindVrijBlok ()
{
    int retBlok;

    if (BlokIsBlokNummer (lastVrij) == 0)
    {
        // lastVrij is niet goed herstel dit
        lastVrij = 1;
    }

    // eerste vanaf het eerste blok
    for (retBlok = lastVrij; retBlok < MAX_NOBLOKS; retBlok++)
    {
        if (mBaanInfo->BlokPointer[retBlok].BlokIONummer == -1)
        {
            // vrij blok gevonden
            lastVrij = retBlok;
            return retBlok;
        }
    }
    for (retBlok = 1; retBlok < lastVrij; retBlok++)
    {
        if (mBaanInfo->BlokPointer[retBlok].BlokIONummer == -1)
        {
            // vrij blok gevonden
            lastVrij = retBlok;
            return retBlok;
        }
    }
    // geen vrij blok meer
    return 0;
}
