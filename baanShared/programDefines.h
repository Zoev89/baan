#if !defined(PROGRAMDEFINES_H)
#define PROGRAMDEFINES_H
#include <cstddef>

// event defines
// verander deze defines alleen in samen werking van het baan programma
#define INTERNAL     0  // voor internal gebruik van het baan programma
#define INIT         1  // nummer is -1 voor globale programma en regelaar nummer voor een regelaar
#define BLOK         2
#define IO           3
#define TIMER        4
#define MUIS         5
#define USER_DEFINED 6  // voor eigen verzonnen events

// note event en nummer worden in het programma gecast naar een short (16bits)


#define TIMER_EENMALIG  0
#define TIMER_HERHALEND 1
#define TIMEOUT_MAX 2147483647

#define IOAANVRAAG_TOGGLE -1
#define IOAANVRAAG_DEFAULT -2
#define IOAANVRAAG_REFRESH 0x800000  // alleen intern gebruik
#define IOGEWIJGERD 1
#define IOTIMEOUT   2

#define BLOKAANVRAAG_BELEG 1
#define BLOKAANVRAAG_VRIJ  2

#define ENGELSEWISSEL_KRUIS 13
#define ENGELSEWISSEL_BUIG  12

#define WISSEL_RECHT 12
#define WISSEL_BUIG  13

#define DRIEWEGWISSEL_12 12
#define DRIEWEGWISSEL_13 13
#define DRIEWEGWISSEL_14 14

#define LAMP_UIT 0
#define LAMP_AAN 1

#define SNELHEID_LANGZAAM 64
#define SNELHEID_RIJDEN   65

#define RICHTING_VOORUIT     1
#define RICHTING_ACHTERUIT   2
#define RICHTING_ONVERANDERD 3

// defines voor de blokStop function
#define ZET_STOP             0
#define VERWIJDER_STOP       1
#define VERWIJDER_ALLE_STOPS 2

// defines voor blokStatus
#define STATUS_VRIJ       0
#define STATUS_VOORUIT    1
#define STATUS_ACHTERUIT  2

#ifdef __cplusplus
//extern "C" {
#endif

int ioAanvraag (int adres, int bit, int stand, int timeout);
void muisEvent (int lbx, int lby, int rox, int roy, int nummer);
void timer (int ticks, int event, int flag);
void deleteTimer (int event);
void zetSpanning (int adres, int spanning, int returnGewenst,
            int prioriteit);
void zetBit (int adres, int bit, int waarde, int returnGewenst,
           int prioriteit);
int leesSpanning (int adres);
int leesBit (int adres, int bit);
void schrijfGetal (int x, int y, int getal, int deeltal, int deler,
             int aantal);
int zetSnelheid (int snelheid);
int blokAanvraag (int adres, int beleg, int timeout);
void verwijderAanvragen ();
int richting (int richt);
int blokStop(int blok, int status);
int blokStatus(int blok);
bool zoekEnBeleg(int blokNummer, bool tegenRichting = false);
int GetTreinBlok(const char *station, const char *spoor, bool tegenRichting = false);
int GetStopBlok(const char *station, const char *spoor, bool tegenRichting = false);
bool IsBlokAanwezig(const char *station, const char *spoor, int blok);
void HerstelStaticData(void *data, size_t size);
void SaveStaticData(void *data, size_t size);

#ifdef __cplusplus
//}
#endif


#endif
