#include "IWissel.h"
#include "IBlok.h"
#include "Rectangle.h"
#include <vector>
#include <array>

#ifndef IIOBITS_H
#define IIOBITS_H

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
    IOBits_t();

    std::vector<RouteringKnoopPunt_t> routeKnoopPunt;
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

  std::vector<BlokPointer_t> StopBlokPointer;
  std::vector<Blok_t> StopBlok;

  int hardwareAdres;		// op welk basis adres zit de IO
  int hardwareBit;		// op welk bit is de IO opgeslagen
  int hardwareReturnWaarde;

  int Tijd;
  int TijdTeller;		/* counter zolang de spoel bekrachtigd word -1 is oneindig */
  int Stand;
  Rectangle rec;

  virtual int Init(const char *Input, std::function<std::string()> newInput) override;
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
  void InitConstructor(BlokPointer_t &EindBlokPointer); // nadat de afgeleide klasse de vectors opzet dan deze algemene initializatie


};


#endif // IIOBITS_H
