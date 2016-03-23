// baan.h : main header file for the BAAN application
//

#if !defined(_BAAN_H)
#define _BAAN_H

//
// helaas de C11 standaard heeft geen make_unique wel een make_shared (vergeten) hier is een implemetatie van
// http://herbsutter.com/gotw/_102/
// aangezien mijn gcc 4.8.4 geen 2014 support heeft doen we het maar zo
//
#include <memory>
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}


#define MAX_FILENAME 1024


#define CYCLETIME 50

/* interne snelheid in de regelaars is 8 maal hoger dan extern */
#define SNELHEID_SHIFT 3

#define BAAN_BLOK   0
#define WISSEL_BLOK 1
#define STOP_BLOK   2

#define MAX_AANTAL_REGELAARS   15
#define MAX_MICRO_MEM          11	// het maximum aantal transfers per cycle (min MAX_AANTAL_REGELAARS)
// we kunnen meer regelaars hebben maar er worden er maar een beperkt aantal aangestuurd
#define MAX_ACTIVE_REGELAARS 8

// De huidige code is fixed op MAX_AANTAL_REGELAARS als MAX_MICRO_MEM kleiner is
// dan kan er memory corruption ontstaan als de regelaar aansturing nog steeds 
// de aansturing array indexceerd via het regelaanNummer
#define MAX_MICRO_MEM_ALLOCATION (MAX_AANTAL_REGELAARS + MAX_MICRO_MEM - MAX_ACTIVE_REGELAARS)


#define MAX_NOBLOKS           350

// size of regelaar bitmap
#define REGELBITMAP_X 38
#define REGELBITMAP_Y 18


enum {
// Define voor de SetTextWindow call in de regelaars

WM_ECHTE_SNELHEID,  // 0
WM_GEEF_PROGRESS,   // 1

// defines voor de baan view
WM_BLOK_DISPLAY,    // 2
WM_TIMER_VERLOPEN,  // 3
WM_BLOK_EVENT,      // 4
WM_INTERNALTIMER_VERLOPEN, // 5
WM_WISSEL_DISPLAY,         // 6
WM_VIERKANT_DISPLAY        // 7 display een vierkant ergens op het scherm
};

enum {
BLOK_REMOVE = -2,
BLOK_CLEAR = -1,
BLOK_BELEG = 0,
BLOK_KOPBLOK = 1,
BLOK_KORTSLUIT = 2,
BLOK_RESERVED  = 3
};

enum {
BAAN_KLEUR_ZWART
};

#endif // !defined(_BAAN_H)
