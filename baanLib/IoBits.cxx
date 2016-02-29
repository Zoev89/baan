#include "IIoBits.h"
#include "baan.h"
#include "baanWT.h"

IOBits_t::IOBits_t()
{
    hardwareReturnWaarde = 0;
}

void IOBits_t::InitConstructor(BlokPointer_t &EindBlokPointer)
{
    for (int x=0;x<routeKnoopPunt.size();x++)
    {
        routeKnoopPunt[x].wisselNummer = -2;
        routeKnoopPunt[x].knoopPunt = -2;
        routeKnoopPunt[x].richtingVoorkeur = richtingBeiden;
    }
    for (size_t x = 0; x < StopBlokPointer.size(); x++)
    {
        StopBlokPointer[x].pBlok = &StopBlok[x];
        StopBlokPointer[x].pVolgendBlok = &EindBlokPointer;
        StopBlokPointer[x].pVorigBlok = &EindBlokPointer;
        StopBlokPointer[x].Lengte = 1;
        StopBlokPointer[x].BlokIONummer = -1;        // ongebruikt blok
        StopBlokPointer[x].BlokType = WISSEL_BLOK;
        StopBlok[x].State = BLOK_STOP;
        StopBlok[x].Snelheid = 0;
        StopBlok[x].MaxSnelheid = MAX_SNELHEID;
        StopBlok[x].blokSein = GEEN_SEIN;
        StopBlok[x].seinState = SEIN_GROEN;
        StopBlok[x].hardwareSeinState = SEIN_GROEN;
        StopBlok[x].Bovenleiding = 1;

        StopBlok[x].Stop = -1;
        StopBlok[x].RegelaarNummer = -2;
        StopBlok[x].DetectorCount = 0;
        StopBlok[x].richtingVoorkeur = richtingBeiden;

    }
}

int IOBits_t::Init(char *, FILE *)
{
    return 0;
}

void IOBits_t::InitRoutering ()
{

}

void IOBits_t::ZoekOngeinitializeerdeEindBlokken()
{

}

void IOBits_t::Display ()
{

}

int IOBits_t::Aanvraag (int )
{
    return 0;
}

void IOBits_t::String(char *)
{

}

void IOBits_t::NieuwXY (int , int , int , int )
{

}

void IOBits_t::InitDialoog ()
{

}

void IOBits_t::DialoogOk ()
{

}

void IOBits_t::TestIOTimer ()
{

}

float IOBits_t::GetAdres()
{
    return 0;
}

int IOBits_t::CheckWissel()
{
    return 0;
}
void IOBits_t::VeranderBlok (BlokPointer_t *, BlokPointer_t *)
{

}

BlokPointer_t * IOBits_t::GetWisselBlok()
{
    return NULL;
}

int IOBits_t::GetRichting()
{
    return 0;
}

int IOBits_t::WisselLengte(int , int )
{
    return 0;
}


int IOBits_t::CheckWisselBlok (BlokPointer_t * pBlok, int Richting, BlokPointer_t *eindpointer)
{
    int tegenRichting;

    tegenRichting = (Richting + 1) & 1;

    if ((pBlok->blokRicht[Richting] == NULL) ||
            (pBlok->blokRicht[Richting] == eindpointer))
    {
        return 1;
    }
    if (pBlok->blokRicht[Richting]->blokRicht[tegenRichting] != pBlok)
    {
        return 2;
    }
    return 0;
}
float IOBits_t::WisselAdres()
{
    return hardwareAdres + (float)(hardwareBit)/100.0f;
}

