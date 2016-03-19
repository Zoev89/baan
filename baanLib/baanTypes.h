// baanTypes.h : structures type for the hardware besturing
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_BAANTYPES_H)
#define _BAANTYPES_H

#include "baan.h"
#include "programDefines.h"
#include "baanSemaphore.h"
#include "socketCom.h"
#include <Rectangle.h>
#include "spoorInformatie.h"
#include "IIoBits.h"
#include <memory>
#include "IRegelaarViewUpdates.h"
#include "IBaanTreinen.h"
#include "IBaanDoc.h"
#include "IRegelaarInstellingenDialoog.h"
#include "IBaanMessage.h"
#include "array"

// Het aantal stop blokken per wissel; voor een drie weg wissel heb ik er 3 nodig
// Ik zou strikt genomen met 2 stopblokken kunnen volstaan bij een drieweg wissel,
// maar dan is er geen relatie welk stopblok voor welk baan vak gebruikt wordt.

//#define MAX_AANTAL_AANVRAGEN  30
#define MAX_AANTAL_AANVRAGEN  40

#define NO_OF_TIMERS 20
// AANVRAAG_EVENT wordt gebruikt voor het opzetten van de timer
// als de user een timeout in de ioAanvraag heeft gezet
#define AANVRAAG_EVENT 1
#define AANVRAAG_CANCEL 2

// de minimale lengte van blok om mee telaten doen
// kleiner dan dit kan ik toch niet gebruiken om te stoppen
#define MIN_LENGTE 35

typedef struct
{
  pbaanSemaphore_t semWacht;
  int Aanvraag;			/* Zet deze op 1 voor een aanvraag van: */
  int NieuweRegelaar;		/* een nieuwe regelaar                  */
  int VerwijderRegelaar;	/* het verwijderen van een regelaar     */

  int RegelaarNummer;		/* Betreft welke regelaar */

  /*
   ** De volgende parameters zijn alleen van belang voor een 
   ** regelaar aanvraag
   **/
  int BlokNummer;
  int Lengte;
  int Richting;			/* 0 = vooruit 1 = terug */
  int NeemKopBlokMee;		/* 0 = wordt niet meegenomen (default) 1 = wel */
  int AanvraagAccept;		/* 0 = niet geaccepteerd  1 = geaccepteerd */
} RegelaarAanvraag_t;



class BaanZoekResult
{
public:
    BaanZoekResult(float _nummer, 
    int stand1, int stand2, int stand3, BlokPointer_t * next)
    {
        nummer = _nummer;
        stand[0]=stand1;
        stand[1]=stand2;
        stand[2]=stand3;
        nextBlok=next;
    }
    
    float nummer;  // type blok=bloknummer  wissel=adres.bit in IOBits
    int stand[3];   // voor wissel de stand
    BlokPointer_t * nextBlok;
    int aantalMogelijkheden()
    {
        if (stand[0] == 0)
            return 0;
        if (stand[1] == 0)
            return 1;
        if (stand[2] == 0)
            return 2;
        return 3;
    }
    bool isBlok()
    {
        if (stand[0]==1)
            return true;
        return false;
    }
};

/*
** Sorry maar er is geen andere mogelijkheid dan het hier
** tedoen.
**/
#include "regelaar.h"

/*
** Sorry maar regelaar en workthread hebben deze beiden nodig
** Dus Blok_t en Blok_Pointer_t zijn in baan.h gedeclareed
** omdat regelaar.h en BaanWT.h vanelkaar afhankelijk zijn 
**/


struct IOAanvraag_t
{
  int IONummer;
  int stand;			// inwelke stand moet die of toggle
};


typedef struct
{
  int GeefVrijAanvraag;
  int BlokNummer;
} GeefVrijAanvraag_t;


typedef struct
{
  SeinState_t SeinState;
  int richting;
  int snelheid;
  int returnWaarde;
  int detect;
} Simulatie_t;

typedef enum
{
  HW_BLOK,
  HW_IO
} hardwareBIO_t;

typedef struct
{
  hardwareBIO_t blokIO;
  int adres;			// -1 als het niet in gebruik is
  int data;
  // het nummer in de blok of wisselSpoelArray ook altijd invullen (ivm kortsluit test)!
  int nummer;
  int returnGewenst;
} hardwareArray_t;

typedef struct
{
  int aantal;
  hardwareArray_t array[MAX_MICRO_MEM_ALLOCATION];
} hardwareCom_t;

class ChardwareCom
{
  // PASOP de implementatie is niet reenterend!
  // nieuwItem en krijgItem kunnen wel van
  // twee processen gecalled worden
  // Construction
public:
  ChardwareCom ();		// constructor de ~ChardwareCom descructor is niet nodig
  int nieuwItem (hardwareArray_t * data);	// zet een nieuwItem in de array return 1 dan is die vol
  int krijgItem (hardwareArray_t * data);	// krijg het oudste item uit de array return 1 als leeg
  int aantalItems ();
protected:

private:
  int kop;			// kop wijst altijd naar een lege plek
  int staart;
  int maxLengte;
  hardwareArray_t array[MAX_AANTAL_AANVRAGEN];
};

typedef struct
{
  int event;
  int eventType;
  int regelaar;
  int flag;
  int ticks;
  int tickTime;
} baanTimer_t;


struct BaanInfo_t
{
public:
  pbaanSemaphore_t semCriticalSection;
  pbaanSemaphore_t semStarted;

  int StopExecution;
  int SleepTime;		// de tijd die we moeten wachten (typical 50 ms)

  // timers voor de gebruiker van het baan API
  int tickTimer;  // 100ms timer
  baanTimer_t timerArray[NO_OF_TIMERS];

  // handregelaar spul (oud)
  int Stop;			// als stop is dan worden de regelaars op nul gezet.
  // als de stop knop wordt ingedruk dan moeten we actie ondernemen.
  // Als die stopknop dan weer wordt ingedrukt mag de stop conditie
  // afgesloten worden.
  int vorigeStopStatus;

  // hetzelfde als de stop maar nu via het grafische interface
  int noodStop;

  int Achteruit;		// het achteruit commando
  int Vooruit;			// het vooruit commando

  // voor het versturen van data waarbij we alleen
  // geinteresseerd zijn in de recieve data van vorige keer
  // Helaas moet dit adres wel een gebruikt adres zijn
  int idleBlokCounter;

  // variablenen voor simulatie
  int Simulatie;
  // het memory van de centrale micro voor debuging
  short centraleMicroMem[2 * MAX_MICRO_MEM];
  // het memory van de controllers zelf
  Simulatie_t SimulatieArray[MAX_NOBLOKS];
  // Einde variabelen voor simulatie


  hardwareCom_t hardwareDetection;	// detection verzend data
  hardwareCom_t hardwareSnelheid;	// snelheid verzend data

  ChardwareCom hardwareHoog;	// hoge prioritijd IO afhandeling
  ChardwareCom hardwareLaag;	// lage prioritijd IO afhandeling

  pbaanSemaphore_t semWorkerThreadStopped;	// einde semaphore


  int AantalBlokken;
  int AantalSpoelen;

  std::vector<Regelaar> RegelArray;//[MAX_AANTAL_REGELAARS];


  BlokPointer_t BlokPointer[MAX_NOBLOKS];
  Blok_t Blok[MAX_NOBLOKS];
  Blok_t EindBlok;
  BlokPointer_t EindBlokPointer;

  std::vector<std::unique_ptr<IOBits_t>> IOBits;

  RegelaarAanvraag_t RegelaarAanvraag;
  GeefVrijAanvraag_t GeefVrijAanvraag;
  
  SpoorInformatie spoorInfo;

  char blkDir[MAX_FILENAME];
  char *blkName;

  // variable gebruikt voor editMode
  int editMode;
  int selectedBlok;
  IOBits_t *selectedWissel = NULL;
  int selectedWisselPoint = -1;
  int selectedWisselX;
  int selectedWisselY;

  int selectedOffsetX;
  int selectedOffsetY;

  // Voor een progress bar om te zien hoe het met de aanvraag array gesteld is
  int debugAantalItemsX;
  int debugAantalItemsY;

  // programma td events die ik nog hier laat anders heeft elk programma deze events
  int tdProgInternal;
  int tdProgBlok;
  int tdProgIO;
  int tdProgTimer;
  int tdProgMuis;
  int tdProgUserDefined;


};




#endif // !defined(_BAANTYPES_H)
