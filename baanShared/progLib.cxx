#include "progLib.h"
#include "programDefines.h"
#include "programma.h"

static class CProgramma *instantie;
//static programCallbacks_t *callbacks;


//
// Deze routine wordt door het besturings programma
// aangeroepen om de interface pointer te zetten
//
extern "C" void
zetInstantie (class CProgramma * inst)
{
  instantie = inst;
}

int
ioAanvraag (int adres, int bit, int stand, int timeout)
{
  return instantie->ioAanvraag (adres, bit, stand, timeout);
}

void
muisEvent (int lbx, int lby, int rox, int roy, int nummer)
{
  instantie->muisEvent (lbx, lby, rox, roy, nummer);
}

void
timer (int ticks, int event, int flag)
{
  instantie->timer (ticks, event, flag, TIMER);
}

void
deleteTimer (int event)
{
  instantie->deleteTimer (event);
}

void
zetSpanning (int adres, int spanning, int returnGewenst, int prioriteit)
{
  instantie->zetSpanning (adres, spanning, returnGewenst, prioriteit);
}

void
zetBit (int adres, int bit, int waarde, int returnGewenst, int prioriteit)
{
  instantie->zetBit (adres, bit, waarde, returnGewenst, prioriteit);
}

int
leesSpanning (int adres)
{
  return instantie->leesSpanning (adres);
}

int
leesBit (int adres, int bit)
{
  return instantie->leesBit (adres, bit);
}

void
schrijfGetal (int x, int y, int getal, int deeltal, int deler, int aantal)
{
  instantie->schrijfGetal (x, y, getal, deeltal, deler, aantal);
}

int
zetSnelheid (int snelheid)
{
  return instantie->zetSnelheid (snelheid);
}

int
blokAanvraag (int adres, int beleg, int timeout)
{
  return instantie->blokAanvraag (adres, beleg, timeout);
}

void
verwijderAanvragen ()
{
  instantie->verwijderAanvragen ();
}

int
richting (int richt)
{
  return instantie->richting (richt);
}

int
blokStop (int blok, int status)
{
  return instantie->blokStop (blok, status);
}

int
blokStatus (int blok)
{
  return instantie->blokStatus (blok);
}

bool zoekEnBeleg(int blokNummer, bool tegenRichting)
{
  return instantie->zoekEnBeleg (blokNummer, tegenRichting);
}

int GetTreinBlok(const char *station, const char *spoor, bool tegenRichting)
{
  return instantie->GetTreinBlok (station, spoor, tegenRichting);
}

int GetStopBlok(const char *station, const char *spoor, bool tegenRichting)
{
  return instantie->GetStopBlok (station, spoor, tegenRichting);
}

bool IsBlokAanwezig(const char *station, const char *spoor, int blok)
{
  return instantie->IsBlokAanwezig (station, spoor, blok);
}

void HerstelStaticData(void *data, size_t size)
{
    return instantie->HerstelStaticData(data,size);
}

void SaveStaticData(void *data, size_t size)
{
    return instantie->SaveStaticData(data,size);
}
