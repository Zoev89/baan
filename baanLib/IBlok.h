#ifndef IBLOK_H
#define IBLOK_H
#include <string>
#include <functional>

// De volgoorde van de items in deze enum mag niet veranderd worden. Ik
// gebruik dat voor de seinen.
typedef enum
{
  SEIN_GROEN,
  SEIN_GEEL,
  SEIN_ROOD,
  // Tja eigenlijk niet in gebruik maar het kan handig
  // zijn om vanuit een gebruiker programma een sein code op te drukken die
  // niet verdwijnt
  SEIN_VAST_GROEN,
  SEIN_VAST_GEEL,
  SEIN_VAST_ROOD,
} SeinState_t;

typedef enum
{
  GEEN_SEIN,
  ROOD_GROEN_SEIN,
  ROOD_GROEN_GEEL_SEIN
} blokSein_t;

enum BlokRichtingVoorkeur
{
  richtingVooruit = 1,
  richtingAchteruit = 2,
  richtingBeiden = 3
};


typedef struct Blok_t
{
  std::function<void(void)> specialProcessing; // call this function when the blok is used (draaischijf)

  int DetectorCount;		/* counter die aangeeft hoevaak een detect al is geweest */

  // de hardware status
  int hardwareAdres;
  int hardwareState;
  int hardwareReturnWaarde;
  int nulWaarde;		// de waarde van de spanning als er niets in het blokzit (DC waarde)

  // Voor de state waarden zie de baanWT.h
  int State;			// het status van het blok in de database
  int Snelheid;
  int MaxSnelheid;		/* MaxSnelheid geeft de maximum snelheid voor dit blok op */
  int RegelaarNummer;		/* -1 is unused, -2 used of stopblok */

  blokSein_t blokSein;		// geeft aan wat voor een seinpaal er op dit blok staat

  SeinState_t seinState;	// de tijdelijke waarde die voor de hardware gebruikt wordt
  SeinState_t hardwareSeinState;	// de uiteindelijke waarde die voor de hardware gebruikt wordt
  // stop conditie op een blok
  // hier wordt het regelaar nummer in gestopt voor als die moet stoppen
  // default initializatie = -1
  // Door deze constructie kan ik met een functie alle stop settings
  // weghalen door te zoeken naar het regelaar nummer in het stop blok
  int Stop;			/* Trein moet stoppen */
  int Bovenleiding;		/* 1=bovenleiding 0=geen bovenleiding */
  BlokRichtingVoorkeur richtingVoorkeur;  // wordt voor baanZoek gebruikt voor een exta penalty

  int XCoord;
  int YCoord;

  int blokBelegt;		// voor het beleggen van blokken in user programmaas
  int blokBelegtRegelaar;	// om hem vrij te kunnen geven
} Blok_t;

enum
{
  VOLGENDBLOK,
  VORIGBLOK
};

typedef struct BlokPointer_t
{
  union
  {
    struct
    {
      BlokPointer_t *pVolgendBlok;
      BlokPointer_t *pVorigBlok;
    };
    BlokPointer_t *blokRicht[2];
  };
  Blok_t *pBlok;
  // In BlokIONummer staat de index in de blok of IO array
  // BlokIONummer wordt gebruikt voor opslag van de huidige status
  // van de baan. Ook wordt deze variabele gebruikt om te testen of
  // een blok/IO gebruikt is tijdens het controleren van de database
  int BlokIONummer;		// alleen voor opslag en restore werk -1 voor ongebruikt

  /* Blok types
   ** BAAN_BLOK   0
   ** WISSEL_BLOK 1
   ** STOP_BLOK   2
   **/
  int BlokType;

  int Lengte;			/* lengte van een blok */
} BlokPointer_t;


class IBlok
{
public:
    virtual char *BlokNaam(char *string,BlokPointer_t *blok) = 0;
    virtual void BlokDisplay(int blokNummer, int regelaar,
                             int actie) = 0;
    virtual void BlokInsert(BlokPointer_t *newBlok) = 0;
    virtual void BlokDelete(BlokPointer_t *delBlok) = 0;
    virtual void BlokEndPointDelete(BlokPointer_t * delBlok,int richting) = 0;
    virtual void BlokEndPointInsert(BlokPointer_t * insBlok,int richting) = 0;
    virtual void BlokPrint(BlokPointer_t * blok) = 0;
    virtual int BlokIsBlokNummer(int blokNummer) = 0; // return 0 if ok else 1
    virtual int BlokIsVrij(int blokNummer) = 0;  // return 1 if vrij
    virtual int BlokVindVrijBlok() = 0; // return blok nummer, 0=error
};

#endif // IBLOK_H
