#ifndef PROGRAMMA_H
#define PROGRAMMA_H

#include "baanTypes.h"
#include "wissel.h"
#include "IMessage.h"
#include "IBaanMessage.h"
#include "IMainWindowDrawing.h"
#include "ITd.h"
#include "IErrorPrint.h"

#define NO_MUIS_EVENTS 20

typedef  void (*progMain_t)(int event,int nummer);
typedef void (*progAanvraagRet_t)(int adres,int bit,int ret);
typedef void (*zetInstantie_t)(class CProgramma *inst);

typedef struct
{
    int *adres;
    int size;
} varArray_t, *pvarArray_t;

typedef struct
{
    int lbx;
    int lby;
    int rox;
    int roy;
    int nummer;
} muisEvents_t;

typedef struct
{
    int adres;
    int bit;
    int stand;
    int timeout;
} aanVraag_t;

#define AANVRAAG_DIEPTE 16
#define AANVRAAG_MASK 0x0f

class CProgramma
{
public:
    CProgramma (IMessage& message, IWissels& wissels, IBaanMessage &baanMessage, IMainWindowDrawing& mainWindowDrawing, ITd& td, IErrorPrint & errorPrint, BaanInfo_t *baanInfo);
    ~CProgramma ();

    //
    // Ik plaats de api routines bovenaan zodat die niet
    // meteen wijzigen als ik iets toevoeg. Als er een wijziging
    // plaats vind moet je eigenlijk ook alle libs bouwen.
    // Door het api eerst te plaatsen word die er iets minder
    // gevoelig erdoor.
    //
    // routines voor het baan API
    // deze zou ik private willen maar de dynamic lib moet ze kunnen
    // aanroepen. Heb ze ook virtual moeten maken want ik
    // wil een vtable hebben in de dynamic library zodat ik
    // direct de classen kan aanroepen. Ik heb ze niet
    // virtual om ze te overloaden. Omdat ik de library
    // isoleer qua memory ivm met static variabelen kan
    // de library geen unresolved externals hebben.
    //
    virtual int ioAanvraag (int adres, int bit, int stand, int timeout);
    virtual void muisEvent (int lbx, int lby, int rox, int roy, int nummer);
    virtual void timer (int ticks, int event, int flag, int eventType);
    virtual void deleteTimer (int event);
    virtual void zetSpanning (int adres, int spanning, int returnGewenst,
                              int prioriteit);
    virtual void zetBit (int adres, int bit, int waarde, int returnGewenst,
                         int prioriteit);
    virtual int leesSpanning (int adres);
    virtual int leesBit (int adres, int bit);
    virtual void schrijfGetal (int x, int y, int getal, int deeltal, int deler,
                               int aantal);
    virtual int zetSnelheid (int snelheid);
    virtual int blokAanvraag (int adres, int beleg, int timeout);
    virtual void verwijderAanvragen ();   // Gebruik deze voor van buiten
    virtual int richting (int richt);
    virtual int blokStop(int blok, int status);  // renamed progBlokStop
    virtual int blokStatus(int blok);
    virtual bool zoekEnBeleg(int blokNummer, bool tegenRichting = false);
    virtual int GetTreinBlok(const char *station, const char *spoor, bool tegenRichting = false);
    virtual int GetStopBlok(const char *station, const char *spoor, bool tegenRichting = false);
    virtual bool IsBlokAanwezig(const char *station, const char *spoor, int blok);
    virtual void HerstelStaticData(void *data, size_t size);
    virtual void SaveStaticData(void *data, size_t size);

    //
    // einde routines voor het baan API
    //



    // preconditoin van InitGloabl en Init is dat programmaNaam gezet is!
    // Bij het herladen word deze naam weer gebruikt!
    int InitGlobal (string filename);
    int Init (pvarArray_t GlobalArray, std::string filename);
    pvarArray_t GetGlobalArray ();
    void unload ();		// verwijder programma en allocated memory
    void executeProgram (int eventType, int nummer);
    void checkMuisEvents (int x, int y);
    boost::filesystem::path programmaNaam;

    virtual void progVerwijderAanvragen ();   // Gebruik deze voor van buiten



private:
    IMessage& mMessage; // hack to initialize the regelProg in a array
    IWissels& mWissels;
    IBaanMessage& mBaanMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    ITd& mTd;
    IErrorPrint & mErrorPrint;
    BaanInfo_t *mBaanInfo;

    int IONummer (int adres, int bit);
    int welkeRichting(bool tegenRichting);

    void executeInstructions (int eventType, int nummer);
    int *adres (int offset);
    int regelaar;
    void *prog_handle;  // handle naar de dynamic library
    progMain_t progMain;
    zetInstantie_t zetInstantie;
    progAanvraagRet_t progAanvraagRet;
    aanVraag_t aanVraag[AANVRAAG_DIEPTE];
    int aanVraagLees;
    int aanVraagSchrijf;
    int timerRunning;

    // data en routines voor het baan API
    muisEvents_t muisEvents[NO_MUIS_EVENTS];
    int geenGlobaalArray;
    int ioAanvraag(IOAanvraag_t aanvraag, int timeout);
    char *m_staticData;
};

#endif
