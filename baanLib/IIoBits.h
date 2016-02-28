#include "IWissel.h"
#include "IBlok.h"
#include "Rectangle.h"
#include <vector>
#include <array>

#ifndef IIOBITS_H
#define IIOBITS_H

#define AANTAL_BLOKS_PER_WISSEL 3
#define AANTAL_ROUTE_AANSLUITINGEN 4

typedef struct
{
    std::vector<int> blokList;  // blok nummers, wisselNummer, wisselNummerAansluiting
    std::vector<int> aansluitingen;
    int wisselNummer; // -1 voor een eindpunt
    int knoopPunt;    // -1 voor een eindpunt
    BlokRichtingVoorkeur richtingVoorkeur;
} RouteringKnoopPunt_t;

/*
** Elke wissel, ontkoppelaar, sein zit hierin
**/
class IOBits_t: public IWissel
{
public:
    IOBits_t(BlokPointer_t &EindBlokPointer);

    RouteringKnoopPunt_t routeKnoopPunt[AANTAL_ROUTE_AANSLUITINGEN];
  /*
   ** 0 ongebruikt
   ** 1 ontkoppel rail
   ** 2 standaard wissel (recht of gebogen)
   ** 3 kruising zonder aandrijving
   ** 4 Engelse wissel
   ** 5 drieweg wissel
   ** 6 lampje
   ** 7 draaischijf
   **/
  int Type;

  std::array<BlokPointer_t, AANTAL_BLOKS_PER_WISSEL> StopBlokPointer;
  std::array<Blok_t, AANTAL_BLOKS_PER_WISSEL> StopBlok;

  int hardwareAdres;		// op welk basis adres zit de IO
  int hardwareBit;		// op welk bit is de IO opgeslagen
  int hardwareReturnWaarde;

  int Tijd;
  int TijdTeller;		/* counter zolang de spoel bekrachtigd word -1 is oneindig */
  int Stand;
  Rectangle rec;

  virtual int Init(char *Input, FILE *file) override;
  virtual void InitRoutering () override;
  virtual void ZoekOngeinitializeerdeEindBlokken() override;
  virtual void Display () override;
  virtual int Aanvraag (int stand) override;
  virtual void String(char *string) override;
  virtual void NieuwXY (int selectionX, int selectionY, int deltaX, int deltaY) override;
  virtual void InitDialoog () override;
  virtual void DialoogOk () override;
  virtual void TestIOTimer () override;
  virtual float GetAdres() override;
  virtual int CheckWissel() override;
  virtual void VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok) override;
  virtual BlokPointer_t * GetWisselBlok() override;
  virtual int GetRichting() override;
  virtual int WisselLengte(int from, int to) override;

  int CheckWisselBlok (BlokPointer_t * pBlok, int Richting, BlokPointer_t *eindpointer);
  float WisselAdres();


};


#endif // IIOBITS_H
