#include "baanOverzicht.h"

BaanOverzicht::BaanOverzicht()
{

}

std::vector<std::string>
BaanOverzicht::GetOverzicht (BaanInfo_t * pInfo)
{
    int i;
    int aantal[8];
    int blokken = 0;
    char inhoud[100];
    int laatsteBlok;

    memset (aantal, 0, sizeof (aantal));

    for (i = 1; i < MAX_NOBLOKS; i++)
    {
        if (pInfo->BlokPointer[i].BlokIONummer > 0)
        {
            // blok gebruikt dus tel op
            aantal[0] += pInfo->BlokPointer[i].Lengte;
            blokken += 1;
            laatsteBlok = i;
        }
    }

    // we zoeken alle spoelen af en maken een tabel per io controller
    for (i = 0; i < pInfo->AantalSpoelen; i++)
    {
        int Type;
        IOBits_t *pWissel;

        pWissel = pInfo->IOBits[i].get();
        Type = pWissel->Type;
        aantal[Type] += 1;
    }

    // tabel opgebouwd nu het displayen
    std::vector<std::string> returnVector;

    sprintf (inhoud, "%dm\trails", aantal[0] / 100);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tblokken", blokken);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tlampjes", aantal[6]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tontkoppelaars", aantal[1]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tstandaard wissels", aantal[2]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tkruisingen", aantal[3]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tengelse wissels", aantal[4]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tdrieweg wissels", aantal[5]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tTotaal wissels", aantal[5] + aantal[2] + aantal[4]);
    returnVector.push_back(inhoud);
    sprintf (inhoud, "%d\tdraaischijven", aantal[7]);
    returnVector.push_back(inhoud);

    for (i = 1; i <= laatsteBlok; i++)
    {
        sprintf (inhoud, "blok %3d\t%s", i,
                 (pInfo->BlokPointer[i].BlokIONummer >
                  0) ? (pInfo->Blok[i].hardwareAdres != i)? "draai blok" : "Gebruikt" : "Vrij");
        returnVector.push_back(inhoud);
    }
    return returnVector;
}
