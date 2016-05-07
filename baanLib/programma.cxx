#include <stdio.h>
#include <string.h>
#include "programma.h"
#include "math.h"
#include "baanWT.h"
#include "td.h"
#include "baanDoc.h"
#include "baanMessage.h"
#include "baanView.h"
#include <dlfcn.h>
#include "errorPrint.h"
#include "baanZoek.h"
#include <boost/format.hpp>
#include "baanUtility.h"



int CProgramma::IONummer (int adres, int bit)
{
    int i;

    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        if (mBaanInfo->IOBits[i].get()->Type)
        {
            if ((mBaanInfo->IOBits[i].get()->hardwareAdres == adres) &&
                    (mBaanInfo->IOBits[i].get()->hardwareBit == bit))
            {
                return i;
            }
        }
    }
    mMessage.message (str(boost::format("adres %d bit %d niet gevonden")% adres% bit));
    return -1;                    // een invalid adres
}


CProgramma::CProgramma (IMessage& message, IWissels& wissels, IBaanMessage& baanMessage, IMainWindowDrawing& mainWindowDrawing, ITd &td, IErrorPrint &errorPrint,  BaanInfo_t *baanInfo) :
    mMessage(message),
    mWissels(wissels),
    mBaanMessage(baanMessage),
    mMainWindowDrawing(mainWindowDrawing),
    mTd(td),
    mErrorPrint(errorPrint),
    mBaanInfo(baanInfo)
{
    int i;

    geenGlobaalArray = 0;
    prog_handle = NULL;
    // initialize de muisEvents tot ongebruikt
    for (i = 0; i < NO_MUIS_EVENTS; i++)
    {
        // dit is een onbelegde plaats
        muisEvents[i].lbx = -1;
        muisEvents[i].lby = -1;
        muisEvents[i].rox = -1;
        muisEvents[i].roy = -1;
        muisEvents[i].nummer = -1;
    }
    regelaar = -1;
    m_staticData = NULL;
    timerRunning = 0;
    aanVraagLees = aanVraagSchrijf = 0;

    unload();
}

CProgramma::~CProgramma ()
{
    unload ();
}

void
CProgramma::unload ()
{
    int i;

    progMain = NULL;
    progAanvraagRet = NULL;
    zetInstantie = NULL;

    if (prog_handle)
    {
        dlclose (prog_handle);
        prog_handle = NULL;
        if (mBaanInfo)
        {
            // remove user created timers
            for (i = 0; i < NO_OF_TIMERS; i++)
            {
                if (mBaanInfo->timerArray[i].ticks != -1)
                {
                    // deze is gebruikt door deze regelaar
                    if (mBaanInfo->timerArray[i].regelaar == regelaar)
                    {
                        // dit is de timer die we zochten
                        mBaanInfo->timerArray[i].ticks = -1;
                    }
                }
            }
            if (regelaar >= 0)
            {
                // voor het verwijderen van regelaars only
                verwijderAanvragen ();
                //                      mBaanInfo->RegelArray[regelaar].zetGewensteSnelheid(1);
                blokStop (1, VERWIJDER_ALLE_STOPS);
            }
        }
    }
    if (m_staticData)
    {
        delete m_staticData;
        m_staticData = NULL;
    }
}

int
CProgramma::InitGlobal (std::string filename)
{
    return Init (NULL, filename);
}

int
CProgramma::Init (pvarArray_t GlobalArray, string filename)
{
    if (filename.empty())
    {
        programmaNaam.clear();
        return 0;
    }
    programmaNaam = filename;
    if (programmaNaam.has_parent_path())
    {
        // hij heeft een dir spec
        if (programmaNaam.parent_path() == mBaanInfo->blkFilePath.parent_path())
        {
            programmaNaam = programmaNaam.filename();
        }
    }
    auto file = programmaNaam.filename();
    if (programmaNaam.has_parent_path())
    {
        file = programmaNaam;
    }
    else
    {
        file = mBaanInfo->blkFilePath.parent_path();
        file /= programmaNaam.filename();
    }
    std::cout<< file << std::endl;
    if (file.filename() == ".")
    {
        return 0;
    }
    // Ik gebruik de dlmopen met LM_ID_NEWLM zodat
    // als men hetzelde programma nog een maal download
    // men normaal dezelfde handel terug krijgt. Dien
    // tengevolge dat de static variablen geshared zijn tussen
    // die 2 regelaars met hele leuke zij effecten.
    // Nu zijn ze weer gescheiden. Let wel op dat als de
    // library unresolved externels heeft dat dit dan wel
    // dodelijk is (kan niet geladen worden).
    //  prog_handle = dlmopen (LM_ID_NEWLM, progNaam, RTLD_LAZY);
    prog_handle = dlopen (file.c_str(), RTLD_LAZY);
    if (!prog_handle)
    {
        auto p=dlerror();
        if (p)
        {
            mErrorPrint.errorPrint ("dlopen error met %s error %s\n", file.c_str(),p);
        }
        else
        {
            mErrorPrint.errorPrint ("dlopen error met %s error\n", file.c_str());
        }
        unload ();
        return 1;
    }


    progMain = (progMain_t) dlsym (prog_handle, "progMain");
    if (progMain == NULL)
    {
        mErrorPrint.errorPrint ("progMain niet gevonden voor %s\n", file.c_str());
        return 2;
    }

    progAanvraagRet =
            (progAanvraagRet_t) dlsym (prog_handle, "progAanvraagRet");

    // optional interfaces
    zetInstantie = (zetInstantie_t) dlsym (prog_handle, "zetInstantie");
    if (zetInstantie == NULL)
    {
        mErrorPrint.errorPrint ("zetInstantie niet gevonden voor %s\n", file.c_str());
        unload ();
        return 3;
    }
    zetInstantie (this);

    // allocate memorys

    if (GlobalArray == NULL)
    {
        //      globalArray.adres = new int[globalArray.size];
        //      if (globalArray.adres == NULL)
        //        {
        //          unload ();
        //          return 1;
        //        }
        //      memset (globalArray.adres, 0, globalArray.size * sizeof (int));
    }
    else
    {
        //      globalArray = *GlobalArray;
        //      geenGlobaalArray = 1;
    }





    // program succesvol geladen
    aanVraagLees = aanVraagSchrijf = 0;
    for (int i = 0; i < AANVRAAG_DIEPTE; i++)
    {
        aanVraag[i].adres = -1;
    }
    timerRunning = 0;

    return 0;
}

pvarArray_t
CProgramma::GetGlobalArray ()
{
    //  return &globalArray;
    return NULL;
}

void
CProgramma::executeProgram (int eventType, int nummer)
{
    // timedoctor debugging
    switch (eventType)
    {
    case INTERNAL:
        mTd.tdValue (mBaanInfo->tdProgInternal, nummer);
        break;
    case BLOK:
        mTd.tdValue (mBaanInfo->tdProgBlok, nummer);
        break;
    case IO:
        mTd.tdValue (mBaanInfo->tdProgIO, nummer);
        break;
    case TIMER:
        mTd.tdValue (mBaanInfo->tdProgTimer, nummer);
        break;
    case MUIS:
        mTd.tdValue (mBaanInfo->tdProgMuis, nummer);
        break;
    case USER_DEFINED:
        mTd.tdValue (mBaanInfo->tdProgUserDefined, nummer);
        break;
    }

    if (eventType == INIT)
    {
        // onthoud het regelaar nummer zodat deze niet
        // in de baan API nodig is
        regelaar = nummer;
        // voor de initializatie van het user programma neem ik het blok nummer
        if (nummer >= 0)
            nummer = mBaanInfo->RegelArray[regelaar].pKopBlok->BlokIONummer;
    }

    if (eventType == INTERNAL)
    {
        timerRunning = 0;
        if (nummer == AANVRAAG_CANCEL)
        {
            int i;
            // zoek in de uitstaande aanvragen of deze er tussen zit
            for (i = 0; i < AANVRAAG_DIEPTE; i++)
            {
                if (aanVraag[i].adres != -1)
                {
                    // deze aanvraag staat nog open
                    aanVraag[i].adres = -1;
                    if (progAanvraagRet)
                    {
                        progAanvraagRet (aanVraag[i].adres, aanVraag[i].bit,
                                         IOGEWIJGERD);
                    }
                }
            }
            aanVraagLees = aanVraagSchrijf = 0;


        }
        if (nummer == AANVRAAG_EVENT)
        {
            int savedBit;
            int savedAdres;
            int savedStand;
            int ret = IOGEWIJGERD;

            do
            {
                ret = IOGEWIJGERD;


                // kijk if deze events nog afgehandeld moet worden
                while ((aanVraagLees != aanVraagSchrijf)
                       && (aanVraag[aanVraagLees].adres == -1))
                {
                    aanVraagLees = (aanVraagLees + 1) & AANVRAAG_MASK;
                }
                if (aanVraagLees == aanVraagSchrijf)
                {
                    // de queue is leeg
                }
                else
                {
                    // we weten zeker nu dat het adres een waarde heeft


                    // behandel de timeout voor een blokAanvraag of
                    // ioAanvraag
                    savedBit = aanVraag[aanVraagLees].bit;
                    savedAdres = aanVraag[aanVraagLees].adres;
                    savedStand = aanVraag[aanVraagLees].stand;
                    if (savedBit == -1)
                    {
                        int state;
                        // het blok word dus aangevraagd
                        baanSemaphoreDown (mBaanInfo->semCriticalSection);
                        state = mBaanInfo->Blok[savedAdres].State;
                        if (((state == BLOK_VRIJ) ||
                             (mBaanInfo->Blok[savedAdres].RegelaarNummer == regelaar) ||
                             (((state == BLOK_VOORUITCHECK)
                               || (state == BLOK_ACHTERUITCHECK))
                              && (mBaanInfo->Blok[savedAdres].Snelheid == 0)))
                                && (mBaanInfo->Blok[savedAdres].blokBelegt == 0))
                        {
                            mBaanInfo->Blok[savedAdres].blokBelegt = 1;
                            mBaanInfo->Blok[savedAdres].blokBelegtRegelaar = regelaar;
                            ret = 0;
                        }
                        baanSemaphoreUp (mBaanInfo->semCriticalSection);
                        // how de criticalSection kort als het effe kan vandaar
                        // de hulp variable
                        if (ret == 0)
                        {
                            mBaanMessage.Post (WM_BLOK_DISPLAY,
                                             savedAdres, regelaar, BLOK_RESERVED);
                        }
                    }
                    else
                    {
                        IOAanvraag_t aanvraag;

                        aanvraag.stand = savedStand;
                        aanvraag.IONummer = IONummer (savedAdres, savedBit);
                        if (aanvraag.IONummer != -1)
                        {
                            ret = mWissels.Aanvraag (&aanvraag);
                        }
                        else
                        {
                            // deze conditie zou niet voorkomen omdat IONummer al eens
                            // eerder is aangeroepen. Maar als het gebeurd dan behandel
                            // het maar als bedient want anders kan die op de timeout
                            // wachten en dat kan lang zijn
                            ret = 0;
                        }
                    }
                    // nu kan ret zijn
                    // ret = 0 dan klaar
                    // ret = IOGEWIJGERD nogmaals proberen
                    if (ret == 0)
                    {
                        if (progAanvraagRet)
                        {
                            progAanvraagRet (savedAdres, savedBit, 0);
                        }
                        aanVraag[aanVraagLees].adres = -1;
                        aanVraagLees = (aanVraagLees) & AANVRAAG_MASK;
                    }
                    else
                    {
                        aanVraag[aanVraagLees].timeout -= 1;
                        if (aanVraag[aanVraagLees].timeout <= 0)
                        {
                            // timeout dus gaan we deze afhandelen
                            if (progAanvraagRet)
                            {
                                progAanvraagRet (savedAdres, savedBit, IOTIMEOUT);
                            }
                            aanVraag[aanVraagLees].adres = -1;
                            aanVraagLees = (aanVraagLees) & AANVRAAG_MASK;
                        }
                        // else hoeven we niets te doen als wachten op de volgende timer
                    }
                }
            }
            // als er 1 is goed gekeurd kijken we meteen naar de volgende aanvraag
            // als die we is
            while (ret == 0);
        }
    }
    else
    {
        // begin execution bij main()
        executeInstructions (eventType, nummer);
        // cleanup de stack
        // het is namelijk niet gegarandeerd dat
        // we bij 0 zijn (INTERNAL events)
    }

    //
    // als er iets in de queue staat dan zetten we de timer als
    // die al niet gezet is
    if ((aanVraagSchrijf != aanVraagLees) && (timerRunning == 0))
    {
        timer (10, AANVRAAG_EVENT, TIMER_EENMALIG, INTERNAL);
        timerRunning = 1;
    }

}

void
CProgramma::executeInstructions (int eventType, int nummer)
{
    if (zetInstantie) zetInstantie(this);

    if (progMain)
    {
        progMain (eventType, nummer);
    }
}


int
CProgramma::ioAanvraag (int adres, int bit, int stand, int timeout)
{
    IOAanvraag_t aanvraag;
    aanvraag.stand = stand;
    aanvraag.IONummer = IONummer (adres, bit);
    return ioAanvraag(aanvraag, timeout);
}


int CProgramma::ioAanvraag(IOAanvraag_t aanvraag, int timeout)
{
    int ret;

    ret = IOGEWIJGERD;
    if (aanvraag.IONummer != -1)
    {
        int adres=mBaanInfo->IOBits[aanvraag.IONummer].get()->hardwareAdres;
        int bit=mBaanInfo->IOBits[aanvraag.IONummer].get()->hardwareBit;
        // een valid adres is gevonden
        // als er een timeout gespecificeerd is en er is
        // iets aangevraagd dan zetten we deze direct in de queue
        // om deadlock te voorkomen tussen regelaars
        if ((aanVraagSchrijf == aanVraagLees) || (timeout == 0))
        {
            ret = mWissels.Aanvraag (&aanvraag);
        }
        if (timeout > 0)
        {

            if (ret == IOGEWIJGERD)
            {
                if (((aanVraagSchrijf + 1) & AANVRAAG_MASK) == aanVraagLees)
                {
                    // queue vol wijger
                    if (progAanvraagRet)
                    {
                        progAanvraagRet (adres, bit, ret);
                    }
                }
                // zet de aanvraag in de queue
                aanVraag[aanVraagSchrijf].adres = adres;
                aanVraag[aanVraagSchrijf].bit = bit;
                aanVraag[aanVraagSchrijf].stand = aanvraag.stand;
                aanVraag[aanVraagSchrijf].timeout = timeout;
                aanVraagSchrijf = (aanVraagSchrijf + 1) & AANVRAAG_MASK;


            }
            else
            {
                if (progAanvraagRet)
                {
                    progAanvraagRet (adres, bit, ret);
                }
            }
        }
    }
    return ret;
}

void
CProgramma::muisEvent (int lbx, int lby, int rox, int roy, int nummer)
{
    int i;
    for (i = 0; i < NO_MUIS_EVENTS; i++)
    {
        if (muisEvents[i].lbx == -1)
        {
            // dit is een onbelegde plaats
            muisEvents[i].lbx = lbx;
            muisEvents[i].lby = lby;
            muisEvents[i].rox = rox;
            muisEvents[i].roy = roy;
            muisEvents[i].nummer = nummer;
            return;
        }
    }
    mMessage.message (str(boost::format("maximum aantal muisEvents %d")% NO_MUIS_EVENTS));
}

void
CProgramma::checkMuisEvents (int x, int y)
{
    int i;
    for (i = 0; i < NO_MUIS_EVENTS; i++)
    {
        if (muisEvents[i].lbx != -1)
        {
            if ((x >= muisEvents[i].lbx) &&
                    (x <= muisEvents[i].rox) &&
                    (y >= muisEvents[i].lby) && (y <= muisEvents[i].roy))
            {
                executeProgram (MUIS, muisEvents[i].nummer);
            }
        }
        else
        {
            return;
        }
    }
}


void
CProgramma::deleteTimer (int event)
{
    int i;

    for (i = 0; i < NO_OF_TIMERS; i++)
    {
        if (mBaanInfo->timerArray[i].ticks != -1)
        {
            // deze is gebruikt
            if ((mBaanInfo->timerArray[i].event == event) &&
                    (mBaanInfo->timerArray[i].regelaar == regelaar))
            {
                // dit is de timer die we zochten
                mBaanInfo->timerArray[i].ticks = -1;
                return;
            }
        }
    }
    // timer niet aangetroffen
    mMessage.message (str(boost::format("timer niet aangetroffen event %d")% event));
}

void
CProgramma::timer (int ticks, int event, int flag, int eventType)
{
    int i;

    // zoek een vrije timer
    if (ticks <= 0)
    {
        // even fool proof maken
        // een negative ticktime is wel iets te klein
        return;
    }
    for (i = 0; i < NO_OF_TIMERS; i++)
    {
        // ik neem aan dat er geen wrap arround ontstaat
        // het programma gebruikt een 100ms tick time
        // dus moet het programma 6 jaar onafgebroken aanstaan
        // voordat een overflow ontstaat
        if (mBaanInfo->timerArray[i].ticks == -1)
        {
            // deze is vrij
            mBaanInfo->timerArray[i].event = event;
            mBaanInfo->timerArray[i].eventType = eventType;
            mBaanInfo->timerArray[i].regelaar = regelaar;
            mBaanInfo->timerArray[i].flag = flag;
            mBaanInfo->timerArray[i].tickTime = mBaanInfo->tickTimer + ticks;
            mBaanInfo->timerArray[i].ticks = ticks;   // initializeer dit het laatst
            return;
        }
    }
    mMessage.message (str(boost::format("maximum aantal timers is %d timer lost")% NO_OF_TIMERS));
}

void
CProgramma::zetSpanning (int adres, int spanning, int returnGewenst,
                         int prioriteit)
{
    hardwareArray_t data;

    if ((adres < 0) || (adres > mBaanInfo->AantalBlokken))
    {
        mMessage.message (str(boost::format("invalid adres %d")% adres));
        return;
    }
    data.data = spanning;
    data.blokIO = HW_BLOK;
    data.adres = adres;
    data.nummer = adres;
    data.returnGewenst = returnGewenst;
    if (prioriteit)
    {
        mBaanInfo->hardwareHoog.nieuwItem (data);
    }
    else
    {
        mBaanInfo->hardwareLaag.nieuwItem (data);
    }
}

void
CProgramma::zetBit (int adres, int bit, int waarde, int returnGewenst,
                    int prioriteit)
{
    hardwareArray_t data;

    data.data = (waarde << 4) + bit;
    data.blokIO = HW_IO;
    data.adres = adres;
    if (data.adres != -1)
    {
        data.nummer = IONummer (adres, bit);
        if (data.nummer == -1)
        {
            mMessage.message (str(boost::format("niet bestaand adres=%d bit=%d")% adres% bit));
            return;
        }

        data.returnGewenst = returnGewenst;
        if (prioriteit)
        {
            mBaanInfo->hardwareHoog.nieuwItem (data);
        }
        else
        {
            mBaanInfo->hardwareLaag.nieuwItem (data);
        }
    }
}

int
CProgramma::leesSpanning (int adres)
{
    if ((adres < 0) || (adres > mBaanInfo->AantalBlokken))
    {
        mMessage.message (str(boost::format("invalid adres %d")% adres));
        return 0;
    }
    return (mBaanInfo->Blok[adres].hardwareReturnWaarde);
}

int
CProgramma::leesBit (int adres, int bit)
{
    int index;
    index = IONummer (adres, bit);
    if (index != -1)
    {
        return (mBaanInfo->IOBits[index].get()->hardwareReturnWaarde);
    }
    return 0;
}

void
getalNaarString (char *string, double getal, int aantal)
{
    char format[10];
    int cyfers;
    double dcyfers;
    int voorKoma = 0;
    int achterKoma = 0;

    if (getal == 0)
    {
        dcyfers = 0;
    }
    else
    {
        dcyfers = log10 (getal);
    }
    if (dcyfers < 0)
    {
        achterKoma = aantal - 2;
        if (achterKoma < 0)
        {
            achterKoma = 0;
        }
        voorKoma = aantal - achterKoma;
    }
    else
    {
        // cyfers voor de coma
        cyfers = (int) dcyfers + 1;
        if (((cyfers + 1) < aantal) && (getal != (int) getal))
        {
            // er is ruimte achter de koma
            voorKoma = cyfers;
            achterKoma = aantal - cyfers - 1;
        }
        else
        {
            // print geen koma dus alleen geheel getal
            voorKoma = aantal;
            achterKoma = 0;
        }
    }
    sprintf (format, "%%-%d.%df", voorKoma, achterKoma);
    sprintf (string, format, getal);
}


void
CProgramma::schrijfGetal (int x, int y, int getal, int deeltal, int deler,
                          int aantal)
{
    char string[20];
    double dGetal;

    dGetal = (double) getal *(double) deeltal / (double) deler;
    getalNaarString (string, dGetal, aantal);
    // clear eerst het scherm!
    mMainWindowDrawing.DisplayString(x,y,string);
}

int CProgramma::zetSnelheid (int snelheid)
{
    int locSnelheid = -1;
    if (regelaar >= 0)
    {
        switch (snelheid)
        {
        case SNELHEID_LANGZAAM:
            snelheid = mBaanInfo->RegelArray[regelaar].Langzaam;
            break;
        case SNELHEID_RIJDEN:
            snelheid = mBaanInfo->RegelArray[regelaar].Rijden;
        }
        // zetGewensteSnelheid clipt de range wel als die fout is
        locSnelheid = mBaanInfo->RegelArray[regelaar].zetGewensteSnelheid (snelheid);
    }
    return locSnelheid;
}


int CProgramma::GetTreinBlok(const char *station, const char *spoor, bool tegenRichting)
{
    return mBaanInfo->spoorInfo.GetTreinBlok(station,spoor,welkeRichting(tegenRichting));
}

int CProgramma::GetStopBlok(const char *station, const char *spoor, bool tegenRichting)
{
    return mBaanInfo->spoorInfo.GetStopBlok(station,spoor,welkeRichting(tegenRichting));
}

bool CProgramma::IsBlokAanwezig(const char *station, const char *spoor, int blok)
{
    return mBaanInfo->spoorInfo.IsBlokAanwezig(station,spoor, blok);
}

int CProgramma::welkeRichting(bool tegenRichting)
{
    int richting = (tegenRichting) ? (mBaanInfo->RegelArray[regelaar].Richting +1 ) & 1:
                                     mBaanInfo->RegelArray[regelaar].Richting;
    richting += RICHTING_VOORUIT;
    return richting;
}

bool CProgramma::zoekEnBeleg(int blokNummer, bool tegenRichting)
{
    int aankomstBlok = blokNummer;
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info, mWissels);
    
    int kopBlok = abs(mBaanInfo->RegelArray[regelaar].pKopBlok->BlokIONummer);
    int richting = welkeRichting(tegenRichting);
    if (tegenRichting)
    {
        aankomstBlok = kopBlok;
        kopBlok = blokNummer;
    }
    std::vector<BaanZoekResultaat> pad = zoek.pad(kopBlok,richting,aankomstBlok,regelaar);
    if (pad.size() == 0)
        return false;
    bool ret = true;
    for(vector<BaanZoekResultaat>::iterator it = pad.begin(), end = pad.end(); (it != end) && (ret) ; ++it)
    {
        if (it->IsWissel())
        {
            IOAanvraag_t aanvraag;
            aanvraag.IONummer = it->m_wisselNummer;
            aanvraag.stand = it->m_stand;
            if (ioAanvraag(aanvraag, 0) == IOGEWIJGERD)
            {
                ret = false;
            };
            if (blokAanvraag(abs(mBaanInfo->IOBits[aanvraag.IONummer].get()->GetWisselBlok()->BlokIONummer),
                             BLOKAANVRAAG_BELEG, 0) == IOGEWIJGERD)
            {
                ret = false;
            }
        }
    }
    if (ret)
    {
        if (blokAanvraag(blokNummer, BLOKAANVRAAG_BELEG, 0) == IOGEWIJGERD)
            ret = false;
    }
    if (ret == false)
    {
        for(vector<BaanZoekResultaat>::iterator it = pad.begin(), end = pad.end(); (it != end) && (ret) ; ++it)
        {
            if (it->IsWissel())
            {
                blokAanvraag(abs(mBaanInfo->IOBits[it->m_wisselNummer].get()->GetWisselBlok()->BlokIONummer),
                        BLOKAANVRAAG_VRIJ, 0);
            }
        }
        blokAanvraag(blokNummer, BLOKAANVRAAG_VRIJ, 0);
    }
    return ret;
}

int
CProgramma::blokAanvraag (int adres, int beleg, int timeout)
{
    int ret = 0;
    if ((adres < 1) || (adres >= MAX_NOBLOKS))
    {
        mErrorPrint.errorPrint ("In prog %s invalid adres %d", programmaNaam.c_str(), adres);
        return IOGEWIJGERD;
    }
    if (beleg == BLOKAANVRAAG_VRIJ)
    {
        int i;

        if (mBaanInfo->Blok[adres].blokBelegtRegelaar == regelaar)
        {
            mBaanInfo->Blok[adres].blokBelegt = 0;
            mBaanInfo->Blok[adres].blokBelegtRegelaar = -1;
            mBaanMessage.Post (WM_BLOK_DISPLAY, adres, regelaar, BLOK_CLEAR);
        }
        // zoek in de uitstaande aanvragen of deze er tussen zit
        for (i = 0; i < AANVRAAG_DIEPTE; i++)
        {
            if ((adres == aanVraag[i].adres) && (aanVraag[i].bit == -1))
            {
                // deze aanvraag staat nog open
                aanVraag[i].adres = -1;
                if (progAanvraagRet)
                {
                    progAanvraagRet (adres, -1, IOGEWIJGERD);
                }
            }
        }
        ret = 0;
    }
    else
    {
        int belegt = 0;
        int state;
        // het blok word dus aangevraagd
        if ((mBaanInfo->Blok[adres].blokBelegt == 1) && (mBaanInfo->Blok[adres].blokBelegtRegelaar == regelaar))
        {
            // blok al aangevraagd door zichzelf gebeurd in het zoek algo wanneer er 2 wissels
            // in 1 blok zitten
            return 0;
        }

        // als er een timeout gespecificeerd is en er is
        // iets aangevraagd dan zetten we deze direct in de queue
        // om deadlock te voorkomen tussen regelaars
        if ((aanVraagSchrijf == aanVraagLees) || (timeout == 0))
        {

            baanSemaphoreDown (mBaanInfo->semCriticalSection);
            state = mBaanInfo->Blok[adres].State;
            if (((state == BLOK_VRIJ) ||
                 (mBaanInfo->Blok[adres].RegelaarNummer == regelaar) ||
                 (((state == BLOK_VOORUITCHECK)
                   || (state == BLOK_ACHTERUITCHECK))
                  && (mBaanInfo->Blok[adres].Snelheid == 0)))
                    && (mBaanInfo->Blok[adres].blokBelegt == 0))
            {
                mBaanInfo->Blok[adres].blokBelegt = 1;
                mBaanInfo->Blok[adres].blokBelegtRegelaar = regelaar;
                belegt = 1;
            }
            baanSemaphoreUp (mBaanInfo->semCriticalSection);
            // hou de criticalSection kort als het effe kan vandaar
        }
        // de hulp variable
        if (belegt == 1)
        {
            ret = 0;
            mBaanMessage.Post (WM_BLOK_DISPLAY, adres, regelaar, BLOK_RESERVED);
            // als er een timeout is opgegeven dan moeten we melden dat het gebeurd is
            if (timeout > 0)
            {
                if (progAanvraagRet)
                {
                    progAanvraagRet (adres, -1, ret);
                }
            }

        }
        else
        {
            ret = IOGEWIJGERD;
            // adres nog niet vrij dus gaan wachten als men dat wil
            if (timeout > 0)
            {
                if (((aanVraagSchrijf + 1) & AANVRAAG_MASK) == aanVraagLees)
                {
                    // queue vol wijger
                    if (progAanvraagRet)
                    {
                        progAanvraagRet (adres, -1, ret);
                    }
                }
                // zet de aanvraag in de queue
                aanVraag[aanVraagSchrijf].adres = adres;
                aanVraag[aanVraagSchrijf].bit = -1;
                aanVraag[aanVraagSchrijf].stand = -1;
                aanVraag[aanVraagSchrijf].timeout = timeout;
                aanVraagSchrijf = (aanVraagSchrijf + 1) & AANVRAAG_MASK;
            }
        }                       // End else if (belegt==1)
    }
    return ret;
}

void
CProgramma::verwijderAanvragen ()
{
    int i;

    // kijk of er nog aanvragen open staan
    if (aanVraagLees != aanVraagSchrijf)
    {
        // omdatverwijderAanvragen vanuit andere threads aangeroepen kan worden
        // ga ik liever door de queue heen. Ander kan ik problemen krijgen
        // want de code is niet thread save
        mBaanMessage.Post (WM_INTERNALTIMER_VERLOPEN,
                         regelaar, AANVRAAG_CANCEL, -1);
    }

    // loop de blok array door om te zien of er nog blokken
    // gereserveerd zijn
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        if ((mBaanInfo->Blok[i].blokBelegt) &&
                (mBaanInfo->Blok[i].blokBelegtRegelaar == regelaar))
        {
            mBaanInfo->Blok[i].blokBelegt = 0;
            mBaanInfo->Blok[i].blokBelegtRegelaar = -1;
            mBaanMessage.Post (WM_BLOK_DISPLAY, i, regelaar, BLOK_CLEAR);
        }
    }
}


// deze routine wordt vanuit het programma aangeroepe
// nu moeten we zeker zijn dat alles weg is na deze call
void
CProgramma::progVerwijderAanvragen ()
{
    int i;

    // kijk of er nog aanvragen open staan
    executeProgram (INTERNAL, AANVRAAG_CANCEL);

    // loop de blok array door om te zien of er nog blokken
    // gereserveerd zijn
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        if ((mBaanInfo->Blok[i].blokBelegt) &&
                (mBaanInfo->Blok[i].blokBelegtRegelaar == regelaar))
        {
            mBaanInfo->Blok[i].blokBelegt = 0;
            mBaanInfo->Blok[i].blokBelegtRegelaar = -1;
            mBaanMessage.Post (WM_BLOK_DISPLAY, i, regelaar, BLOK_CLEAR);
        }
    }
}


int
CProgramma::richting (int richt)
{
    int ret;

    ret = mBaanInfo->RegelArray[regelaar].Richting;
    if (ret == 0)
    {
        ret = RICHTING_VOORUIT;
    }
    else
    {
        ret = RICHTING_ACHTERUIT;
    }
    switch (richt)
    {
    case RICHTING_VOORUIT:
    case RICHTING_ACHTERUIT:
        if (ret != richt)
        {
            // alleen als de richting anders is dan doen we iets
            mBaanInfo->RegelArray[regelaar].RichtingVerandering ();
        }
        break;
    case RICHTING_ONVERANDERD:
        // ret value is al correct niets te doen
        break;
    default:
        ret = 0;

    }
    return ret;
}

int
CProgramma::blokStop (int blok, int status)
{
    int ret = 0;

    if (((blok <= 0) || (blok >= MAX_NOBLOKS)) &&
            ((status == ZET_STOP) || (status == VERWIJDER_STOP)))
    {
        // exit hier verkeerde input
        return 2;
    }

    switch (status)
    {
    case ZET_STOP:
        if ((mBaanInfo->Blok[blok].Stop == -1) ||
                (mBaanInfo->Blok[blok].Stop == regelaar))
        {
            mBaanInfo->Blok[blok].Stop = regelaar;
        }
        else
        {
            // is al belegt door een ander
            ret = 1;
        }
        break;
    case VERWIJDER_STOP:
        if (mBaanInfo->Blok[blok].Stop == regelaar)
        {
            mBaanInfo->Blok[blok].Stop = -1;
        }
        break;
    case VERWIJDER_ALLE_STOPS:
        int i;

        for (i = 1; i < MAX_NOBLOKS; i++)
        {
            if (mBaanInfo->Blok[i].Stop == regelaar)
            {
                mBaanInfo->Blok[i].Stop = -1;
            }
        }
        break;
    default:
        ret = 2;
    }
    return ret;

}

int
CProgramma::blokStatus (int blok)
{
    int bezet = 0;

    if ((blok > 0) && (blok < MAX_NOBLOKS))
    {
        switch (mBaanInfo->Blok[blok].State & BLOK_BEZET_MASK)
        {
        case BLOK_VOORUIT:
        case BLOK_VOORUITCHECK:
            bezet = (mBaanInfo->Blok[blok].RegelaarNummer == regelaar) ? STATUS_VRIJ : STATUS_VOORUIT;
            break;
        case BLOK_ACHTERUIT:
        case BLOK_ACHTERUITCHECK:
            bezet = (mBaanInfo->Blok[blok].RegelaarNummer == regelaar) ? STATUS_VRIJ : STATUS_ACHTERUIT;
            break;
        default:
            bezet = STATUS_VRIJ;
        }
    }
    return bezet;
}

void CProgramma::HerstelStaticData(void *data, size_t size)
{
    if (m_staticData == NULL)
    {
        m_staticData = new char[size];
        memset(m_staticData,0,size);
    }
    memcpy(data,m_staticData,size);
}

void CProgramma::SaveStaticData(void *data, size_t size)
{
    memcpy(m_staticData,data,size);
}
