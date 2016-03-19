#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
# if !defined(__CYGWIN__)
#  include <direct.h>
# endif
# include <windows.h>
#else
# include <unistd.h>
#endif
#include "baanDoc.h"
#include "baanWT.h"
#include "baanMessage.h"
#include "leesdata.h"
#include "td.h"
#include "baanTreinen.h"
#include "blok.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "baanLib.h"
#include "baanUtility.h"

BaanDoc::BaanDoc(IMessage &msg, IBaanWT &baanWT, IBaanTreinen &baanTreinen, IBlok &blok, IWissels& wissels, IMainScreenControls &mainScreenControls, IMainWindowDrawing& mainWindowDrawing, IRegelaarViewUpdates& regelaarViewUpdates,
                 IBaanMessage &baanMessage, ILampInstDialoog &lampInstDialoog, IKopRichtingDialoog &kopRichtingDialoog, INieuwIODialoog &nieuwIODialoog, IAddBlokDialoog& addBlokDialoog,
                 IRegelaarInstellingenDialoog& regelaarInstellingenDialoog,
                 ITelefoonConnectie& telefoonConnectie, ITd &td, IErrorPrint &errorPrint, BaanInfo_t *mBaanInfo) :
    mMessage(msg),
    mMainControls(mainScreenControls),
    mBaanWT(baanWT),
    mBaanTreinen(baanTreinen),
    mBlok(blok),
    mWissels(wissels),
    mMainWindowDrawing(mainWindowDrawing),
    mBaanMessage(baanMessage),
    mLampInstDialoog(lampInstDialoog),
    mKopRichtingDialoog(kopRichtingDialoog),
    mNieuwIODialoog(nieuwIODialoog),
    mAddBlokDialoog(addBlokDialoog),
    mTelefoonConnectie(telefoonConnectie),
    mTd(td),
    mErrorPrint(errorPrint),
    mBaanInfo(mBaanInfo),
    globaalProg(msg, wissels, baanMessage, mainWindowDrawing, td, errorPrint, mBaanInfo)
{
    for (int i=0;i<MAX_AANTAL_REGELAARS;i++)
        regelProg.push_back(CProgramma(msg, wissels, baanMessage, mainWindowDrawing, td, errorPrint, mBaanInfo));

    for (int i=0;i<MAX_AANTAL_REGELAARS;i++)
        mBaanInfo->RegelArray.push_back(Regelaar(regelaarViewUpdates, baanTreinen, *this, regelaarInstellingenDialoog, baanMessage,
                                                telefoonConnectie, td));

    tdglobal = mTd.tdCreate("globalProg");
    mBaanInfo->editMode = 0;
    mBaanInfo->selectedBlok = 0;
    mBaanInfo->selectedWissel = NULL;
    mBaanInfo->selectedWisselPoint = -1;
    mBaanInfo->selectedWisselX = 0;
    mBaanInfo->selectedWisselY = 0;

    mBaanInfo->selectedOffsetX = 0;
    mBaanInfo->selectedOffsetY = 0;
    mBaanInfo->semWorkerThreadStopped = NULL;
    mBaanInfo->RegelaarAanvraag.semWacht = NULL;

}

void
BaanDoc::baanDocOpen (const char *doc)
{
    if (doc)
    {
        FILE *blkFile;


        blkFile = baanDocFileOpen (doc, "rb", mBaanInfo->blkDir, &mBaanInfo->blkName);

        if (blkFile)
        {
            strncpy (blkFileName, doc, MAX_FILENAME - 1);
            printf ("dir <%s>\n", mBaanInfo->blkDir);
            printf ("name <%s>\n", mBaanInfo->blkName);
            boost::filesystem::path path(std::string(mBaanInfo->blkDir) + std::string(mBaanInfo->blkName));
            path.replace_extension("");
            mBaanInfo->spoorInfo.LoadData(path.string());
            mMainControls.mainWindowLabel(mBaanInfo->blkName);
            if (baanDocParseBlkFile (blkFile) == 0)
            {
                printf ("Blk file opened\n");
                // blok file correct dus start het programma
                /*
               ** Begin the worker thread.
               ** First create the semaphores
               ** Then start the worker thread
               **/


                if ((mBaanInfo->RegelaarAanvraag.semWacht == NULL) ||
                        (mBaanInfo->semCriticalSection == NULL))
                {
                    mMessage.message ("Error creating semaphores");
                }
                else
                {
                    if (mBaanMessage.Start())
                    {
                        mMessage.message ("Error starting messageHandler");
                    }
                    else
                    {
                        // start de treinbaan
                        mMainControls.mainWindowOpenDeactivate();
                        mMainControls.mainWindowCloseActivate();
                        //mMainControls.mainWindowRedraw();
                        mMainWindowDrawing.RedrawAll();
                        if (0 == mBaanInfo->editMode)
                        {
                            mBaanInfo->semWorkerThreadStopped =
                                    baanSemaphoreCreate (0);
                            if (mBaanInfo->semWorkerThreadStopped)
                            {
                                mBaanWT.StartWorker();
                                mMainControls.mainWindowTreinActivate();
                                // wacht tot de workthread gestart is
                                baanSemaphoreDown (mBaanInfo->semStarted);
                                baanDocIniFile ();
                                mTelefoonConnectie.Connect();
                            }
                        }
                    }

                }
            }
            EricFgetsClose (blkFile);
            fclose (blkFile);
        }
        else
        {
            blkFileName[0] = 0;   // reset  filename
            mMessage.message (str(boost::format("Error opening this file %s") % doc));
        }
    }
}

void
BaanDoc::baanDocSetWisselsDefault ()
{
    int i;
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        IOAanvraag_t IOAanvraag;
        IOAanvraag.stand = IOAANVRAAG_DEFAULT;
        IOAanvraag.IONummer = i;
        mWissels.Aanvraag (&IOAanvraag);
    }
}

void
BaanDoc::baanDocSchrijfBlkFile ()
{
    int i;
    char string[200];

    // Eerst de wissels in de default stand zetten
    if (blkFileName[0] != 0)
    {
        FILE *blkFile;
        // er is een blk file
        blkFile = baanDocFileOpen (blkFileName, "wb", mBaanInfo->blkDir, &mBaanInfo->blkName);
        if (blkFile)
        {
            int w;

            baanDocSetWisselsDefault ();
            fprintf (blkFile, "# Version\n");
            fprintf (blkFile, "2\n");
            fprintf (blkFile, "# Bitmap filename\n");
            fprintf (blkFile, "%s\n", bitmapBlkFileName);
            fprintf (blkFile, "# ini filename\n");
            fprintf (blkFile, "%s\n", iniBlkFileName);
            fprintf (blkFile, "# global programma naam\n");
            fprintf (blkFile, "Programma = %s\n", globaalProg.programmaNaam);
            fprintf (blkFile, "# aantal blokken en spoelen\n");
            fprintf (blkFile, "%d %d\n", mBaanInfo->AantalBlokken,
                     mBaanInfo->AantalSpoelen);
            fprintf (blkFile,
                     "# positie van de regelaars in linker top x y en rechter bodem x y\n");
            fprintf (blkFile, "%d %d %d %d\n", regelaarTopX, regelaarTopY,
                     regelaarBotX, regelaarBotY);
            fprintf (blkFile,
                     "# positie van het treinen venster linker top x y en breedte hoogte\n");
            fprintf (blkFile, "%d %d %d %d\n", treinenX, treinenY, treinenWidth,
                     treinenHeight);
            fprintf (blkFile, "# coordinaten van het debug progress\n");
            fprintf (blkFile, "%d %d\n", mBaanInfo->debugAantalItemsX,
                     mBaanInfo->debugAantalItemsY);
            fprintf (blkFile, "# vooruit en terug commando voor deze baan\n");
            fprintf (blkFile, "%d %d\n", mBaanInfo->Vooruit, mBaanInfo->Achteruit);

            fprintf (blkFile, "# Nu het aantal blokken\n");
            fprintf (blkFile,
                     "# Nummer Volgend MaxSnelheid Lengte Boven richtingVoorkeur Sein x    y\n");
            for (i = 1; i < MAX_NOBLOKS; i++)
            {
                if (mBaanInfo->BlokPointer[i].BlokIONummer != -1)
                {
                    // blok gebruikt
                    BlokPointer_t *volgend;
                    int MaxSnelheid;
                    volgend = mBaanInfo->BlokPointer[i].pVolgendBlok;
                    mBlok.BlokNaam (string, volgend);
                    MaxSnelheid = mBaanInfo->Blok[i].MaxSnelheid;
                    if (1000 == MaxSnelheid)
                    {
                        MaxSnelheid = -1;
                    }
                    fprintf (blkFile, "%-3d %7s %3d %3d %d %d %d %4d %4d\n", i,
                             string, MaxSnelheid,
                             mBaanInfo->BlokPointer[i].Lengte,
                             mBaanInfo->Blok[i].Bovenleiding,
                             mBaanInfo->Blok[i].richtingVoorkeur,
                             mBaanInfo->Blok[i].blokSein, mBaanInfo->Blok[i].XCoord,
                             mBaanInfo->Blok[i].YCoord);
                }
            }
            fprintf (blkFile, "# Nu het aantal ios\n");
            // schrijf de file weg in oplopende volgoorde leest makkelijk
            for (w = 1; w < LAATSTE_ENTRY; w++)
            {
                // Print eerste even wat voor een soort IO het is
                // Dat maakt de blk file makkelijker leesbaar
                switch (w)
                {
                case 1:
                    fprintf (blkFile, "# onkoppelaars\n");
                    break;
                case 2:
                    fprintf (blkFile, "# standaardWissel\n");
                    break;
                case 3:
                    fprintf (blkFile, "# kruising\n");
                    break;
                case 4:
                    fprintf (blkFile, "# engelseWissel\n");
                    break;
                case 5:
                    fprintf (blkFile, "# driewegWissel\n");
                    break;
                case 6:
                    fprintf (blkFile, "# lamp\n");
                    break;
                }
                for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
                {
                    if (mBaanInfo->IOBits[i].get()->Type == w)
                    {
                        auto str = mWissels.String (i);
                        fprintf (blkFile, "%s\n", str.c_str());
                    }
                }
            }
            fclose (blkFile);


        }
    }
}

void
BaanDoc::baanDocClose ()
{
    if (mBaanInfo->StopExecution == 0)
    {
        int i;
        FILE *file;
        // stop de work thread
        mBaanInfo->StopExecution = 1;
        if (mBaanInfo->editMode)
        {
            baanDocSchrijfBlkFile ();

        }

        if (mBaanInfo->semWorkerThreadStopped)
        {
            baanSemaphoreDown (mBaanInfo->semWorkerThreadStopped);
        }
        // stop de messageHandler
        mBaanMessage.Stop ();

        baanSemaphoreDestroy (mBaanInfo->semWorkerThreadStopped);
        baanSemaphoreDestroy (mBaanInfo->RegelaarAanvraag.semWacht);
        mBaanInfo->RegelaarAanvraag.semWacht = NULL;        // zodat de regelaar er niet meer aankan

        // Nu slaan we de status op van de database
        if (0 == mBaanInfo->editMode)
        {
            // alleen als we in aansturingsmode zitten
            // slaan we de status op
            file = fopen (iniFileName, "wb");
            if (file != NULL)
            {
                fprintf (file, "#Version\n1\n");
                fprintf (file, "# Aantal spoelen\n");
                fprintf (file, "%d\n", mBaanInfo->AantalSpoelen);

                fprintf (file, "# Wissel info\n");
                /* Save de current wissel status */
                for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
                {
                    int Stand;

                    if (mWissels.WisselStand (&Stand, i))
                    {
                        /* hier klopt iets niet we verlaten het saven */
                        i = mBaanInfo->AantalSpoelen;
                    }
                    else
                    {
                        IOBits_t *pWissel = mBaanInfo->IOBits[i].get();

                        fprintf (file, "%d %7.2f %d\n",
                                 pWissel->Type, pWissel->WisselAdres(), Stand);
                    }
                }

                fprintf (file, "# gebruikte regelaars\n");
                for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
                {
                    if (mBaanInfo->RegelArray[i].Gebruikt)
                    {
                        BlokPointer_t *pBlok;
                        int richting, aantalBlokken, tegenRichting;
                        fprintf (file, "%s\n",
                                 mBaanInfo->RegelArray[i].RegelaarFileName);
                        fprintf (file, "%d %d\n",
                                 mBaanInfo->RegelArray[i].Richting,
                                 mBaanInfo->RegelArray[i].Lengte);
                        // bepaal het aantal belegde blokken
                        richting = mBaanInfo->RegelArray[i].Richting;
                        tegenRichting = (richting + 1) & 1;
                        aantalBlokken = 1;
                        pBlok =
                                mBaanInfo->RegelArray[i].
                                pKopBlok->blokRicht[tegenRichting];
                        while ((pBlok->pBlok->State ==
                                (BLOK_VOORUIT + richting))
                               && (pBlok->pBlok->RegelaarNummer == i))
                        {
                            pBlok = pBlok->blokRicht[tegenRichting];
                            aantalBlokken += 1;
                        }
                        /* save welk blok het kop blok is + wat voor een type blok het is */
                        fprintf (file, "%d %d %d\n",
                                 mBaanInfo->RegelArray[i].
                                 pKopBlok->BlokType,
                                 mBaanInfo->RegelArray[i].
                                 pKopBlok->BlokIONummer, aantalBlokken);
                    }
                }
                fclose (file);
            }

            // De status van de baan is opgeslagen nu de regelaars verwijderen
            for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
            {
                if (mBaanInfo->RegelArray[i].Gebruikt)
                {
                    // voordat we de regelaar opheffen zullen we eerst
                    // het programma stoppen
                    regelProg[i].unload ();
                    mBaanInfo->RegelArray[i].CleanUp ();
                }
            }
        }
        mBaanInfo->AantalBlokken = 0;
        mBaanInfo->AantalSpoelen = 0;

        mBaanTreinen.baanDestroyTreinen ();
        mMainControls.mainWindowOpenActivate();
        mMainControls.mainWindowCloseDeactivate();
        mMainControls.mainWindowTreinDeactivate();
        mMainControls.mainWindowRedraw();
        mErrorPrint.errorPrintClose ();
    }
}

FILE *
BaanDoc::baanDocFileOpen (const char *filename, const char *acces, char *dirName,
                          char **name)
{
    FILE *file;
    file = fopen (filename, acces);
    if (file)
    {
        char *p;
        // the file is there so start openening the rest
        strncpy (dirName, filename, MAX_FILENAME);
        p = strrchr (dirName, '/');
        if (p == NULL)
        {
            p = strrchr (dirName, '\\');
        }
        if (p)
        {
            const char *s;
            // split the full name into dir and name
            p[1] = 0;
            s = strrchr (filename, *p);
            strcpy (&p[2], &s[1]);
            *name = &p[2];
        }
        else
        {
            int len;
            // filename did not have a directory spec
            getcwd (dirName, MAX_FILENAME);
            len = strlen (dirName);
            dirName[len] = '/';
            dirName[len + 1] = 0;
            strncpy (&dirName[len + 2], filename, MAX_FILENAME - len - 2);
            *name = &dirName[len + 2];
        }
    }
    return file;
}

std::string BaanDoc::baanDocGetBitmapname()
{
    return std::string(bitmapBlkFileName);
}


int
BaanDoc::baanDocParseBlkFile (FILE * file)
{
    char Array[MAX_FILENAME + 1];
    int i;
    long position;
    int lineCount;
    int version;

    if (EricFgets (Array, MAX_FILENAME, file) == NULL)
    {
        mMessage.message(str(boost::format("Regel %d: EOF Fout in lezen van het versie nummer") %
                             EricFgetsGetLineCount (file)));
        return 1;
    }
    if (sscanf (Array, "%d", &version) != 1)
    {
        mMessage.message(str(boost::format("Regel %d: Kan het versie nummer niet lezen") %
                             EricFgetsGetLineCount (file)));
        return 1;
    }


    /*
   ** Lees de bitmap filename.
   **/
    if (EricFgets (Array, MAX_FILENAME, file) == NULL)
    {
        mMessage.message(str(boost::format
                             ("Regel %d: Fout in lezen van bitmap filename van blok file") %
                             EricFgetsGetLineCount (file)));
        return 1;
    }
    if (baanDocFileName (Array, Array, mBaanInfo->blkDir))
    {
        mMessage.message(str(boost::format("Regel %d: bitmap filename %s niet aanwezig") %
                             EricFgetsGetLineCount (file)% Array));
        return 1;
    }
    strcpy (bitmapBlkFileName, Array);  // save het gehele path
    // vertel de main UI welke bitmap file er gebruikt wordt
    mMainWindowDrawing.SetBitmap(std::string(bitmapBlkFileName));

    /*
   ** Lees de ini filename
   **/
    if (EricFgets (Array, 200, file) == NULL)
    {

        mErrorPrint.errorPrint ("Regel %d: ini filename kan niet gelezen worden",
                    EricFgetsGetLineCount (file));
        return 1;
    }

    if ((':' == Array[1]) || ('/' == Array[0]) || ('\\' == Array[0]))
    {
        // a full path is specified
        strcpy (iniFileName, Array);
    }
    else
    {
        int len;
        strcpy (iniFileName, mBaanInfo->blkDir);
        len = strlen (mBaanInfo->blkDir);
        strcpy (&iniFileName[len], Array);
    }
    strcpy (iniBlkFileName, Array);

    //
    // Lees de global programma naam
    //
    if (EricFgets (globaalProg.programmaNaam, 200, file) == NULL)
    {
        mErrorPrint.errorPrint ("Regel %d: programma filename kan niet gelezen worden",
                    EricFgetsGetLineCount (file));
        return 1;
    }
    if (globaalProg.InitGlobal ())
    {
        mErrorPrint.errorPrint
                ("Regel %d: Het global programma %s kan niet geladen worden",
                 EricFgetsGetLineCount (file), globaalProg.programmaNaam);
        return 1;
    }

    /*
   ** Lees het aantal blokken + het aantal wissel en
   ** Ontkoppel spoelen
   **/
    if (EricFgets (Array, 200, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF Kan het aantal blokken en spoelen niet lezen") %
                     EricFgetsGetLineCount (file)));
        return 1;
    }

    if (sscanf
            (Array, "%d%d", &mBaanInfo->AantalBlokken, &mBaanInfo->AantalSpoelen) != 2)
    {
        mMessage.message (str(boost::format("Regel %d: Kan het aantal blokken en spoelen niet lezen") %
                              EricFgetsGetLineCount (file)));
        return 1;
    }
    if ((mBaanInfo->AantalBlokken < HARDWARE_MIN_ADRES) ||
            (mBaanInfo->AantalBlokken >= MAX_NOBLOKS))
    {
        mMessage.message (str(boost::format("Regel %d: Incorrect aantal blokken %d [%d..%d]") %
                              EricFgetsGetLineCount (file)% mBaanInfo->AantalBlokken%
                              HARDWARE_MIN_ADRES% MAX_NOBLOKS));
        return 1;
    }
    if ((mBaanInfo->AantalSpoelen < 0) || (mBaanInfo->AantalSpoelen >= MAX_NOBLOKS))
    {
        mMessage.message (str(boost::format("Regel %d: Incorrect aantal spoelen %d [%d..%d]") %
                              EricFgetsGetLineCount (file)% mBaanInfo->AantalBlokken%
                              HARDWARE_MIN_ADRES% MAX_NOBLOKS));
        return 1;
    }

    mBaanWT.InitWorkThread ();

    // Lees de default positie van de regelaars
    if (EricFgets (Array, 200, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF Kan de regelaar positie niet lezen") %
                              EricFgetsGetLineCount (file)));
        return 1;
    }
    if (sscanf
            (Array, "%d%d%d%d", &regelaarTopX, &regelaarTopY, &regelaarBotX,
             &regelaarBotY) != 4)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht Topx TopY en BottomX BottomY getallen")%
                     EricFgetsGetLineCount (file)));
        return 1;
    }

    // Lees de grote van het lokomotieven venster
    if (EricFgets (Array, 200, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF Grote en plaats van het locomotieven venster niet lezen") %
                     EricFgetsGetLineCount (file)));
        return 1;
    }
    if (sscanf
            (Array, "%d%d%d%d", &treinenX, &treinenY, &treinenWidth,
             &treinenHeight) != 4)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht X Y en breedte hoogte van het locomotieven venster") %
                     EricFgetsGetLineCount (file)));
        return 1;
    }

    if (0 == mBaanInfo->editMode)
    {
        mBaanTreinen.baanCreateTreinen (treinenX, treinenY, treinenWidth, treinenHeight);
    }

    //
    // Lees het x y coordinaat van de progress
    //
    if (EricFgets (Array, 200, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF x en y van de progress bars niet lezen") %
                              EricFgetsGetLineCount (file)));
        return 1;
    }

    if (sscanf
            (Array, "%d%d", &mBaanInfo->debugAantalItemsX,
             &mBaanInfo->debugAantalItemsY) != 2)
    {
        mMessage.message (str(boost::format("Regel %d: Verwacht X Y van de progress bars") %
                              EricFgetsGetLineCount (file)));
        return 1;
    }

    //
    // Lees de vooruit achteruit commandos
    //
    if (EricFgets (Array, 200, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF Verwacht vooruit achteruit commands") %
                              EricFgetsGetLineCount (file)));
        return 1;
    }

    if (sscanf (Array, "%d%d", &mBaanInfo->Vooruit, &mBaanInfo->Achteruit) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht twee getallen voor vooruit en achteruit commands") %
                     EricFgetsGetLineCount (file)));
        return 1;
    }

    // onthoud de huidige positie!
    position = ftell (file);
    lineCount = EricFgetsGetLineCount (file);

    // scan de blok database om te zien welke blokken er gebruikt worden
    // daarna doen we de wissels. Pas als alle adressen bekend en goed gekeurd zijn
    // gaan we pas het echte initializatie werk doen.
    // In de blokdefinities kunnen ook wissels zitten en daar weten we het adres nog niet
    // van. Vandaar deze eerste scan actie
    for (i = 0; i < mBaanInfo->AantalBlokken; i++)
    {
        int Nummer;

        if (EricFgets (Array, 200, file) == NULL)
        {
            mMessage.message (str(boost::format("Regel %d: End of file tijdens bloken lezen") %
                                  EricFgetsGetLineCount (file)));
            return 1;
        }
        if (sscanf (Array, "%d", &Nummer) != 1)
        {
            mMessage.message (str(boost::format("Regel %d: Incorrect blok string at index %d\n%s") %
                                  EricFgetsGetLineCount (file)% i% Array));
            return 1;
        }
        if (mBlok.BlokIsBlokNummer (Nummer) == 0)
        {
            mMessage.message
                    (str(boost::format("Regel %d: Blok %d niet binnen de range van 1 tot %d\n%s") %
                         EricFgetsGetLineCount (file)% Nummer% MAX_NOBLOKS% Array));
            return 1;
        }
        // check ook of dit blok ook nieuw is
        if (mBaanInfo->BlokPointer[Nummer].BlokIONummer != -1)
        {
            mMessage.message (str(boost::format("Regel %d: Blok %d bestaat al!\n%s") %
                                  EricFgetsGetLineCount (file)% Nummer% Array));
            return 1;
        }

        mBaanInfo->BlokPointer[Nummer].BlokIONummer = Nummer;       // geeft aan dat die gebruikt is
    }

    //
    // Initializeer de wissel database met adres en subadres
    // pas als dat gebeurd is kunnen we de wissel gaan inlezen.
    // Het kan namelijk voorkomen dat twee wissel aanelkaar
    // zitten en dat dus een voorwaard referentie naar de andere wissel
    // nog niet gelezen is.
    //
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        int type, x;
        int hardwareAdres, hardwareBit;
        float adres;
        IOBits_t *pWissel;

        if (EricFgets (Array, 200, file) == NULL)
        {
            mMessage.message (str(boost::format("Regel %d: End of file ben op %d verwacht %d") %
                                  EricFgetsGetLineCount (file)% i%
                                  mBaanInfo->AantalSpoelen));
            return 1;
        }
        if (sscanf (Array, "%d%f", &type, &adres) != 2)
        {
            mMessage.message
                    (str(boost::format("Regel %d: Probleem lezen van type adres van wissel %d\n%s") %
                         EricFgetsGetLineCount (file)% i% Array));
            return 1;
        }
        hardwareAdres = (int) (adres + 0.5);
        hardwareBit = (int) ((adres - (float) hardwareAdres) * 100.0 + 0.5);
        if ((hardwareAdres < HARDWARE_MIN_ADRES) ||
                (hardwareAdres > HARDWARE_MAX_ADRES) ||
                (hardwareBit < HARDWARE_IO_LSB_BITNUMMER) ||
                (hardwareBit > HARDWARE_IO_MSB_BITNUMMER))
        {
            mMessage.message (str(boost::format("Regel %d: Ongeldig IO adres %d.%d!\n%s") %
                                  EricFgetsGetLineCount (file)% hardwareAdres%
                                  hardwareBit% Array));
            return 1;
        }
        // Controlleer of dit adres al gebruikt is als blok
        for (x = 0; x < MAX_NOBLOKS; x++)
        {
            if (mBaanInfo->BlokPointer[x].BlokIONummer != -1)
            {
                // blok is gebruikt
                if (mBaanInfo->BlokPointer[x].BlokIONummer == hardwareAdres)
                {
                    mMessage.message
                            (str(boost::format("Regel %d: IO adres %d is al een belegd!\n%s") %
                                 EricFgetsGetLineCount (file)% hardwareAdres% Array));
                    return 1;
                }
            }
        }
        for (x = 0; x < i; x++)
        {
            if ((mBaanInfo->IOBits[x].get()->hardwareAdres == hardwareAdres) &&
                    (mBaanInfo->IOBits[x].get()->hardwareBit == hardwareBit))
            {
                mMessage.message
                        (str(boost::format("Regel %d IO adres %d.%02d wordt al gebruikt!\n%s") %
                             EricFgetsGetLineCount (file)% hardwareAdres% hardwareBit%
                             Array));
                return 1;
            }
        }

        // creer de wissel class van het correcte type zodat de blok database de blokpointers kan gebruiken voor de initializatie
        mWissels.CreateNewIO(type);
        pWissel = mBaanInfo->IOBits[i].get();

        pWissel->hardwareAdres = hardwareAdres;
        pWissel->hardwareBit = hardwareBit;
    }

    //
    // terug naar het begin van de blokken
    //
    fseek (file, position, SEEK_SET);
    EricFgetsSetLineCount (file, lineCount);

    /*
   ** Lees nu de blok database
   **/
    for (i = 0; i < mBaanInfo->AantalBlokken; i++)
    {
        int Nummer, MaxSnelheid, Lengte, Boven, blokSein, x, y;
        int voorKeurRichting;
        char BlokType[2];
        float Volgend;
        BlokPointer_t *pVolgend;

        BlokType[0] = 0;
        BlokType[1] = 0;
        if (EricFgets (Array, 200, file) == NULL)
        {
            mMessage.message (str(boost::format("Regel %d: End of file tijdens bloken lezen") %
                                  EricFgetsGetLineCount (file)));
            return 1;
        }
        if (version <= 1)
        {
            if (sscanf (Array, "%d%1s%f%d%d%d%d%d%d", &Nummer, BlokType, &Volgend,
                        &MaxSnelheid, &Lengte, &Boven, &blokSein, &x, &y) != 9)
            {
                mMessage.message (str(boost::format("Regel %d: Incorrect blok string at index %d\n%s") %
                                      EricFgetsGetLineCount (file)% i% Array));
                return 1;
            }
            voorKeurRichting = richtingBeiden;
        }
        else
        {
            if (sscanf (Array, "%d%1s%f%d%d%d%d%d%d%d", &Nummer, BlokType, &Volgend,
                        &MaxSnelheid, &Lengte, &Boven, &voorKeurRichting, &blokSein, &x, &y) != 10)
            {
                mMessage.message (str(boost::format("Regel %d: Incorrect blok string at index %d\n%s") %
                                      EricFgetsGetLineCount (file)% i% Array));
                return 1;
            }
        }
        if (mBlok.BlokIsBlokNummer (Nummer) == 0)
        {
            mMessage.message
                    (str(boost::format("Regel %d: Blok %d niet binnen de range van 1 tot %d\n%s") %
                         EricFgetsGetLineCount (file)% Nummer% MAX_NOBLOKS% Array));
            return 1;
        }
        // Hoef eigenlijk niet te checken of deze al belegd is of niet want het kan alleen dezelfde
        // zijn die we eerder gezien hebben. Maar je weet maar nooit
        // met programmeer fouten....
        if (mBaanInfo->BlokPointer[Nummer].BlokIONummer != Nummer)
        {
            mMessage.message
                    (str(boost::format("Regel %d: Blok %d is incorrect geinitializeerd bug in baan?\n%s") %
                         EricFgetsGetLineCount (file)% Nummer% Array));
            return 1;
        }

        pVolgend = mWissels.wisselKrijgPointer (BlokType[0], Volgend);


        if (NULL == pVolgend)
        {
            mMessage.message
                    (str(boost::format("Regel %d: Volgend blok %s%g niet gevonden in de database\n%s") %
                         EricFgetsGetLineCount (file)% Array));
            return 1;
        }
        // test of de pVolgendBlok NULL is
        if (&mBaanInfo->EindBlokPointer !=
                mBaanInfo->BlokPointer[Nummer].pVolgendBlok)
        {
            // we hebben een probleem
            mMessage.message
                    (str(boost::format("Regel %d: Van Blok %d is het volgend blok al belegd met blok %d\n%s") %
                         EricFgetsGetLineCount (file)% Nummer%
                         mBaanInfo->BlokPointer[Nummer].pVolgendBlok->BlokIONummer% Array));
            return 1;
        }


        if (Volgend == -1)
            mBaanInfo->BlokPointer[Nummer].pVolgendBlok = &mBaanInfo->EindBlokPointer;
        else
        {
            // eerst de heenweg
            mBaanInfo->BlokPointer[Nummer].pVolgendBlok = pVolgend;
            // nu de terug weg
            // test of de pVorigBlok van het  pVolgendBlok NULL is
            if (&mBaanInfo->EindBlokPointer != pVolgend->pVorigBlok)
            {
                // we hebben een probleem
                mMessage.message
                        (str(boost::format("Regel %d: Van Blok %d is het volgend blok zijn TERUG weg al belegd met blok %d\n%s") %
                             EricFgetsGetLineCount (file)% Nummer%
                             pVolgend->pVorigBlok->BlokIONummer% Array));
                return 1;
            }

            pVolgend->pVorigBlok = &mBaanInfo->BlokPointer[Nummer];
        }


        mBaanInfo->BlokPointer[Nummer].Lengte = Lengte;

        if (MaxSnelheid >= 0)
            mBaanInfo->Blok[Nummer].MaxSnelheid = MaxSnelheid;
        mBaanInfo->Blok[Nummer].Bovenleiding = Boven;
        mBaanInfo->Blok[Nummer].blokSein = (blokSein_t) blokSein;
        mBaanInfo->Blok[Nummer].XCoord = x;
        mBaanInfo->Blok[Nummer].YCoord = y;
        mBaanInfo->Blok[Nummer].richtingVoorkeur = (BlokRichtingVoorkeur)voorKeurRichting;
    }





    /*
   ** Lees nu de wissel database
   **/
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        int err;

        if (EricFgets (Array, 200, file) == NULL)
        {
            mMessage.message (str(boost::format("Regel %d End of file ben op %d verwacht %d") %
                                  EricFgetsGetLineCount (file)% i%
                                  mBaanInfo->AantalSpoelen));
            return 1;
        }
        if ((err = mWissels.Init (i, Array, [&]()
        {
          char temp[MAX_FILENAME + 1];
          EricFgets (temp, 200, file);
          return std::string(temp);
        })) != 0)
        {
            switch (err)
            {
            case WISSEL_ERR_NIET_ALLES_AANWEZIG:
                mMessage.message
                        (str(boost::format("Regel %d Niet alle parameters aanwezig van wissel %d.%02d\n%s") %
                             EricFgetsGetLineCount (file)%
                             mBaanInfo->IOBits[i].get()->hardwareAdres%
                             mBaanInfo->IOBits[i].get()->hardwareBit% Array));
                break;
            case WISSEL_ERR_INVALID_ADRES:
                mMessage.message
                        (str(boost::format("Regel %d: Een van de adressen is incorrect van wissel %d.%02d\n%s"
                                           "\ncheck ook aantal spoelen in blk file") %
                             EricFgetsGetLineCount (file)%
                             mBaanInfo->IOBits[i].get()->hardwareAdres%
                             mBaanInfo->IOBits[i].get()->hardwareBit% Array));
                break;
            case WISSEL_ERR_INVALID_TYPE:
                mMessage.message (str(boost::format("Regel %d: Onbekend IO type\n%s") %
                                      EricFgetsGetLineCount (file)% Array));
                break;
            case WISSEL_ERR_MESSAGE_AL_GEGEVEN:
                break;
            default:
                mMessage.message
                        (str(boost::format("Regel %d: Onbekende fout bij wissel %d.%02d\n%s") %
                             EricFgetsGetLineCount (file)%
                             mBaanInfo->IOBits[i].get()->hardwareAdres%
                             mBaanInfo->IOBits[i].get()->hardwareBit% Array));
                break;
            }
            if (mBaanInfo->editMode == 0)
            {
                // voor normale excutie laat ik dit niet toe maar in editmode wel
                return 1;
            }
        }
    }

    i = baanDocCheckDatabase ();
    if (mBaanInfo->editMode)
    {
        // ik laat fouten toe in editmode
        i = 0;
    }
    if (i!=0)
        return i;

    // initializeer de routering database
    // elke wissel heeft op elke aansluiting een knoop informatie. Daar kan je vinden welke aansluitingen
    // er zijn of welke blokken er in het vervolg zitten.
    // Het idee is om de database niet vanuit de blokken te beschrijven maar vanuit de wissel aansluitingen.
    // Op elke aansluitingen kan je een rij blokken vinden maar die eindigt altijd in een wissel aansluiting
    // of een eindblok.
    for (i=0;i < mBaanInfo->AantalSpoelen;i++)
    {
        mWissels.InitRoutering (i);
    }
    for (i=0;i < mBaanInfo->AantalSpoelen;i++)
    {
        mWissels.ZoekOngeinitializeerdeEindBlokken(i);
    }
    return 0;

}

int
BaanDoc::baanDocCheckDatabase ()
{
    int i;

    // check de consistency van de database
    // Note ook al komt het programma door deze check heen wil het zeker nog
    // niet zeggen dat de database goed is! Het programma kan namelijk
    // niet weten wat je bedoelt hebt het kan alleen controleren
    // of alles een valid aansluiting heeft.
    //
    // Check1
    //     kijk of je het volgende/vorige blok wel geinitalizeerd is
    // Check2
    //     kijk of je het volgende/vorige blok naar jouw terug wijst
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        if (mBaanInfo->BlokPointer[i].BlokIONummer != -1)
        {
            // blok gebruikt
            if (mBaanInfo->BlokPointer[i].pVolgendBlok !=
                    &mBaanInfo->EindBlokPointer)
            {
                if (mBaanInfo->BlokPointer[i].pVolgendBlok->pVorigBlok == NULL)
                {
                    mMessage.message
                            (str(boost::format("Blok %d volgend blok is niet in de database") % i));
                    return 1;
                }
                if (mBaanInfo->BlokPointer[i].pVolgendBlok->pVorigBlok !=
                        &mBaanInfo->BlokPointer[i])
                {
                    mMessage.message
                            (str(boost::format("Blok %d volgend blok wijst niet terug naar dit blok (%d)")%
                                 i%
                                 mBaanInfo->BlokPointer[i].pVolgendBlok->
                                 pVorigBlok->BlokIONummer));
                    return 1;
                }
            }
            // Nu dezelfede check in omgekeerde richting
            if (mBaanInfo->BlokPointer[i].pVorigBlok != &mBaanInfo->EindBlokPointer)
            {
                if (mBaanInfo->BlokPointer[i].pVorigBlok->pVolgendBlok == NULL)
                {
                    mMessage.message (str(boost::format("Blok %d vorig blok is niet in de database") %
                                          i));
                    return 1;
                }
                if (mBaanInfo->BlokPointer[i].pVorigBlok->pVolgendBlok !=
                        &mBaanInfo->BlokPointer[i])
                {
                    mMessage.message
                            (str(boost::format("Blok %d vorig blok wijst niet terug naar dit blok(%d)") %
                                 i%
                                 mBaanInfo->BlokPointer[i].pVorigBlok->
                                 pVolgendBlok->BlokIONummer));
                    return 1;
                }
            }
        }
    }

    // check nu of alle wissel goed zijn aangesloten
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        if (mWissels.CheckWissel (i))
        {
            return 1;
        }
    }
    return 0;

}


void
BaanDoc::baanDocLeftMouseButtonDown (int x, int y)
{
    int i;

    // De linker muis knop is ingedrukt
    // Eerst kijken of het de noodknop is
    if ((x >= mBaanInfo->EindBlok.XCoord) &&
            (x <= mBaanInfo->EindBlok.XCoord + RESET_SIZE) &&
            (y >= mBaanInfo->EindBlok.YCoord) &&
            (y <= mBaanInfo->EindBlok.YCoord + RESET_SIZE))
    {
        mBaanInfo->noodStop = 1;
    }

    // Controlle of een spoel zich aangesproken voelt
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        if (mBaanInfo->IOBits[i].get()->Type)
        {
            if ((x >= mBaanInfo->IOBits[i].get()->rec.x ()) &&
                    (x <= mBaanInfo->IOBits[i].get()->rec.r ()) &&
                    (y >= mBaanInfo->IOBits[i].get()->rec.y ()) &&
                    (y <= mBaanInfo->IOBits[i].get()->rec.b ()))
            {
                IOAanvraag_t IOAanvraag;

                // Het gegeven punt ligt in deze wissel dus roep
                // de workthread aan met een wissel request
                IOAanvraag.stand = IOAANVRAAG_TOGGLE;
                IOAanvraag.IONummer = i;
                mWissels.Aanvraag (&IOAanvraag);

                i = mBaanInfo->AantalSpoelen;
            }
        }
    }

    /* Hierna gaan we de blokken testen of er een lengte aanvraag is */
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        if (mBaanInfo->BlokPointer[i].BlokIONummer != -1)
        {
            // blok gebruikt
            if ((mBaanInfo->Blok[i].State == BLOK_VOORUIT) ||
                    (mBaanInfo->Blok[i].State == BLOK_ACHTERUIT))
            {
                if ((x >= mBaanInfo->Blok[i].XCoord) &&
                        (x <= mBaanInfo->Blok[i].XCoord + REGELBITMAP_X) &&
                        (y >= mBaanInfo->Blok[i].YCoord) &&
                        (y <= mBaanInfo->Blok[i].YCoord + REGELBITMAP_Y))
                {
                    /*
                   ** Het gegeven punt ligt in deze wissel dus roep
                   ** de workthread aan met een wissel request
                   **/
                    mBaanInfo->GeefVrijAanvraag.BlokNummer = i;
                    mBaanInfo->GeefVrijAanvraag.GeefVrijAanvraag = 1;
                    i = MAX_NOBLOKS;
                }
            }
        }
    }
    // test voor muis events in het globale programma
    globaalProg.checkMuisEvents (x, y);

    // test voor muis events voor de regelaars
    for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
    {
        if ((mBaanInfo->RegelArray[i].Gebruikt) &&
                (mBaanInfo->RegelArray[i].programRunning))
        {
            regelProg[i].checkMuisEvents (x, y);
        }
    }

    /* Geef de input focus aan de active regelaar */
    // TODO port this
    //      if (mBaanInfo->RegelArray[m_mBaanInfo->HandRegelaar.Index].Gebruikt)
    //      {
    //              mBaanInfo->RegelArray[m_mBaanInfo->HandRegelaar.Index].SetForegroundWindow();
    //      }

}

void
BaanDoc::baanDocRightMouseButtonDown (int x, int y)
{
    int i;
    // De rechter muis knop is ingedrukt check of we
    // een lamp dialoog kunnen oppoppen
    for (i = 0; i < mBaanInfo->AantalSpoelen; i++)
    {
        if ((mBaanInfo->IOBits[i].get()->Type == LAMP) || (mBaanInfo->IOBits[i].get()->Type == DRAAISCHIJF) || (mBaanInfo->editMode))
        {

            int w, h;
            w = mBaanInfo->IOBits[i].get()->rec.w ();
            h = mBaanInfo->IOBits[i].get()->rec.h ();
            if (mBaanInfo->editMode)
            {
                // in edit mode wil ook de lampen die niet
                // onder user control zitten kunnen instellen
                w = abs (w);
                h = abs (h);
            }
            if ((x >= mBaanInfo->IOBits[i].get()->rec.x ()) &&
                    (x <= (mBaanInfo->IOBits[i].get()->rec.x () + w)) &&
                    (y >= mBaanInfo->IOBits[i].get()->rec.y ()) &&
                    (y <= (mBaanInfo->IOBits[i].get()->rec.y () + h)))
            {
                if (mBaanInfo->editMode)
                {
                    mBaanInfo->selectedWissel = mBaanInfo->IOBits[i].get();
                    mBaanInfo->selectedOffsetX = x;
                    mBaanInfo->selectedOffsetY = y;
                    mBaanInfo->selectedWisselX = x;
                    mBaanInfo->selectedWisselY = y;
                    mBaanInfo->selectedWissel->Display();

                }
                else
                {
                    // Het gegeven punt is van deze lamp
                    // Dus popup de instellinge van de lamp
                    if (mWissels.InitDialoog(i))
                    {
                        mWissels.DialoogOk(i);
                    }
                }
                i = mBaanInfo->AantalSpoelen;
            }
        }
    }

    /* Hierna gaan we de blokken testen of een regelaar een show nodig heeft */
    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        if (mBaanInfo->BlokPointer[i].BlokIONummer != -1)
        {
            // blok gebruikt
            if ((mBaanInfo->Blok[i].RegelaarNummer != -1) || (mBaanInfo->editMode))
            {
                if ((x >= mBaanInfo->Blok[i].XCoord) &&
                        (x <= mBaanInfo->Blok[i].XCoord + REGELBITMAP_X) &&
                        (y >= mBaanInfo->Blok[i].YCoord) &&
                        (y <= mBaanInfo->Blok[i].YCoord + REGELBITMAP_Y))
                {
                    /*
                   ** Het gegeven punt ligt in dit blok dus roep
                   ** show aan
                   **/
                    if (mBaanInfo->editMode)
                    {
                        mBaanInfo->selectedBlok = i;
                        mBaanInfo->selectedOffsetX = x - mBaanInfo->Blok[i].XCoord;
                        mBaanInfo->selectedOffsetY = y - mBaanInfo->Blok[i].YCoord;
                        mBlok.BlokDisplay (i, -1, BLOK_CLEAR);
                    }
                    else
                    {

                        baanDocPlaatsRegelaar (mBaanInfo->Blok[i].RegelaarNummer, true);
                    }

                    i = MAX_NOBLOKS;
                }
            }

            /* Geef de input focus aan de active regelaar */
            // TODO port this
            //      if (mBaanInfo->RegelArray[m_mBaanInfo->HandRegelaar.Index].Gebruikt)
            //      {
            //              mBaanInfo->RegelArray[m_mBaanInfo->HandRegelaar.Index].SetForegroundWindow();//      }
        }
    }
}

int BaanDoc::baanDocInitRegelaar (int RegelaarNummer, int show)
{
    char BitmapFileName[MAX_FILENAME];
    FILE *file;
    float Alfa;
    float Snelheid1, Afstand1, Snelheid2, Afstand2;
    int TopSnelheid;
    char *punt;
    char *streep1;
    char *streep2;
    int version;
    // haal eerst het blkDir deel van de naam af als dat bestaat
    if (strncmp(mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName, mBaanInfo->blkDir,strlen(mBaanInfo->blkDir)) == 0)
    {
        strcpy(mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName,&mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName[strlen(mBaanInfo->blkDir)]);
    }
    if (baanDocFileName (BitmapFileName, mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName, mBaanInfo->blkDir))
    {
        return 1;
    }
    file =
            fopen (BitmapFileName, "rb");
    if (file == NULL)
    {
        return 1;
    }
    // herleid de locNaam van de filenaam
    punt =
            strrchr (mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName, '.');

    // zoek naar de laatste directory /
    // daar ik unix en dos style doorelkaar heb zoek ik naar beiden

    streep1 =
            strrchr (mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName,
                     '/') + 1;
    streep2 =
            strrchr (mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName,
                     '\\') + 1;
    if (streep1 > streep2)
    {
        strncpy (mBaanInfo->RegelArray[RegelaarNummer].locNaam, streep1,
                 punt - streep1);
        mBaanInfo->RegelArray[RegelaarNummer].locNaam[punt - streep1] = 0;
    }
    else
    {
        if (streep1 < streep2)
        {
            strncpy (mBaanInfo->RegelArray[RegelaarNummer].locNaam, streep2,
                     punt - streep2);
            mBaanInfo->RegelArray[RegelaarNummer].locNaam[punt - streep2] =
                    0;
        }
        else
        {
            // de \ en de / kwamen niet voor dus we hebben temaken met een kale locnaam
            strncpy (mBaanInfo->RegelArray[RegelaarNummer].locNaam,
                     mBaanInfo->RegelArray[RegelaarNummer].RegelaarFileName,
                     punt -
                     mBaanInfo->RegelArray[RegelaarNummer].
                     RegelaarFileName);
            mBaanInfo->RegelArray[RegelaarNummer].locNaam[punt -
                    mBaanInfo->RegelArray
                    [RegelaarNummer].RegelaarFileName]
                    = 0;

        }
    }


    /* Lees de bitmap in */
    sprintf (BitmapFileName, "%s.xpm",
             mBaanInfo->RegelArray[RegelaarNummer].locNaam);
    if (baanDocFileName (BitmapFileName, BitmapFileName, mBaanInfo->blkDir))
    {
        fclose (file);
        return 1;
    }


    mBaanInfo->RegelArray[RegelaarNummer].bitmapName = BitmapFileName;

    // Lees de version van de loc file
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen van het versie nummer") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d", &version) != 1)
    {
        mMessage.message (str(boost::format("Regel %d: Verwacht een getal als versie nummer") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }

    /*
   ** Lees of het een eloc is
   **/
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen van het eloc bit") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d",
                &mBaanInfo->RegelArray[RegelaarNummer].ELoc) != 1)
    {
        mMessage.message (str(boost::format("Regel %d: Verwacht een getal 0 of 1 als eloc") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }


    /*
   ** Lees de minimum en maximum snelheid
   **/
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF bij het lezen van minimum en maximum snelheid") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d%d",
                &mBaanInfo->RegelArray[RegelaarNummer].MinSnelheid,
                &mBaanInfo->RegelArray[RegelaarNummer].MaxSnelheid) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht twee getallen minimum en maximum snelheid") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }

    /*
   ** Lees de top snelheid in km/h
   **/
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen de topsnelheid") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d", &TopSnelheid) != 1)
    {
        mMessage.message (str(boost::format("Regel %d: Verwacht een getal voor de topsnelheid") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    mBaanInfo->RegelArray[RegelaarNummer].TopSnelheidKmh = TopSnelheid;
    mBaanInfo->RegelArray[RegelaarNummer].TopSnelheid =
            (int) (63.0 / (float) TopSnelheid * 16384 + 0.5);


    /*
   ** Lees de alfa an clip voor versnel en vertraag
   **/
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF bij het lezen van alfa en clip voor vertragen") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%g%d",
                &Alfa,
                &mBaanInfo->RegelArray[RegelaarNummer].NormaalClip) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht 2 getallen alfa en clip voor vertragen") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    mBaanInfo->RegelArray[RegelaarNummer].fNormaalAlfa = Alfa;
    mBaanInfo->RegelArray[RegelaarNummer].NormaalAlfa =
            (int) (Alfa * 32768);
    mBaanInfo->RegelArray[RegelaarNummer].NormaalClip <<= SNELHEID_SHIFT;

    /*
   ** Lees de alfa an clip voor stoppen
   **/
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF bij het lezen van alfa en clip voor stoppen") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%g%d",
                &Alfa,
                &mBaanInfo->RegelArray[RegelaarNummer].StopClip) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht 2 getallen alfa en clip voor stoppen") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    mBaanInfo->RegelArray[RegelaarNummer].fStopAlfa = Alfa;
    mBaanInfo->RegelArray[RegelaarNummer].StopAlfa = (int) (Alfa * 32768);
    mBaanInfo->RegelArray[RegelaarNummer].StopClip <<= SNELHEID_SHIFT;


    /* Lees de remweg tabel */
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF bij het lezen van remweg snelheid1 en afstand1") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%g%g", &Snelheid1, &Afstand1) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht 2 getallen voor remweg snelheid1 en afstand1")  %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message
                (str(boost::format("Regel %d: EOF bij het lezen van remweg snelheid2 en afstand2") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%g%g", &Snelheid2, &Afstand2) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht 2 getallen voor remweg snelheid2 en afstand2") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    mBaanInfo->RegelArray[RegelaarNummer].InitRemWeg (Snelheid1, Afstand1,
                                                     Snelheid2, Afstand2);

    //mBaanInfo->RegelArray[RegelaarNummer].view.properties->
    //         image (mBaanInfo->RegelArray[RegelaarNummer].bitmapName);
    // Bij InitDialog neem ook bitmapName mee!!!!
    mBaanInfo->RegelArray[RegelaarNummer].OnInitDialog (RegelaarNummer);
    baanDocPlaatsRegelaar (RegelaarNummer, show);


    // lees het programma
    if (EricFgets (regelProg[RegelaarNummer].programmaNaam, 200, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen van het programma naam") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    //  mBaanInfo->RegelArray[RegelaarNummer].pView = m_mBaanInfo->pView;
    int ret = regelProg[RegelaarNummer].Init (globaalProg.GetGlobalArray ());
    if (ret == 0)
        regelProg[RegelaarNummer].executeProgram (INIT, RegelaarNummer);

    // lees de langzaam en rijden variabelen
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen van langzaam en rijden") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d%d",
                &mBaanInfo->RegelArray[RegelaarNummer].Langzaam,
                &mBaanInfo->RegelArray[RegelaarNummer].Rijden) != 2)
    {
        mMessage.message
                (str(boost::format("Regel %d: Verwacht twee getallen voor langzaam en rijden") %
                     EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    // lees het Loc type in
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen van het loctype naam") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    strncpy (mBaanInfo->RegelArray[RegelaarNummer].locSoort, BitmapFileName,
             LOCSOORT_SIZE);

    // lees de laatsteWagonCheck
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen van het laatsteWagon bit") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d",
                &mBaanInfo->RegelArray[RegelaarNummer].laatsteWagonCheck) !=
            1)
    {
        mMessage.message (str(boost::format("Regel %d: Verwacht 0 of 1 voor het laatsteWagon bit") %
                              EricFgetsGetLineCount (file)));
        mBaanInfo->RegelArray[RegelaarNummer].laatsteWagonCheck = 0;
    }

    if (version == 1)
    {
        float gainFactor;
        float difMul;
        float alpha;

        // Lees de gainFactor difMul alpha plusMinus
        // van de lastafhankelijke regeling
        if (EricFgets (BitmapFileName, 256, file) == NULL)
        {
            mMessage.message
                    (str(boost::format("Regel %d: EOF bij het lezen gainFactor difMul alpha plusMinus lastafhankelijke regeling") %
                         EricFgetsGetLineCount (file)));
            EricFgetsClose (file);
            fclose (file);
            return 1;
        }
        if (sscanf (BitmapFileName, "%f %f %f %d\n",
                    &gainFactor,
                    &difMul,
                    &alpha,
                    &mBaanInfo->RegelArray[RegelaarNummer].plusMinus) != 4)
        {
            // TODO disable de PID snelheid regeling
        }
        mBaanInfo->RegelArray[RegelaarNummer].lastGain1 = gainFactor;
        mBaanInfo->RegelArray[RegelaarNummer].lastGain2 = gainFactor;
        mBaanInfo->RegelArray[RegelaarNummer].lastStand1 = 0;
        mBaanInfo->RegelArray[RegelaarNummer].lastStand2 = 1;

    }
    else
    {
        // Version 2 of the loc files
        if (EricFgets (BitmapFileName, 256, file) == NULL)
        {
            mMessage.message
                    (str(boost::format("Regel %d: EOF bij het lezen lastRegelKeuze lastGain1 lastGain2 lastStand1 en lastStand2 lastafhankelijke regeling")%
                         EricFgetsGetLineCount (file)));
            EricFgetsClose (file);
            fclose (file);
            return 1;
        }
        if (sscanf (BitmapFileName, "%d %f %f %d %d\n",
                    &mBaanInfo->RegelArray[RegelaarNummer].lastRegelKeuze,
                    &mBaanInfo->RegelArray[RegelaarNummer].lastGain1,
                    &mBaanInfo->RegelArray[RegelaarNummer].lastGain2,
                    &mBaanInfo->RegelArray[RegelaarNummer].lastStand1,
                    &mBaanInfo->RegelArray[RegelaarNummer].lastStand2) != 5)
        {
            // TODO disable de PID snelheid regeling
        }

        if (EricFgets (BitmapFileName, 256, file) == NULL)
        {
            mMessage.message
                    (str(boost::format("Regel %d: EOF bij het lezen Lowpass PlusMinus Helling en Dodetijd van de lastafhankelijke regeling") %
                         EricFgetsGetLineCount (file)));
            EricFgetsClose (file);
            fclose (file);
            return 1;
        }
        if (sscanf (BitmapFileName, "%f %d %f %f\n",
                    &mBaanInfo->RegelArray[RegelaarNummer].k_lpf,
                    &mBaanInfo->RegelArray[RegelaarNummer].plusMinus,
                    &mBaanInfo->RegelArray[RegelaarNummer].helling,
                    &mBaanInfo->RegelArray[RegelaarNummer].dodeTijd) != 4)
        {
            // TODO disable de PID snelheid regeling
        }
    }

    // Lees de totalen uit
    if (EricFgets (BitmapFileName, 256, file) == NULL)
    {
        mMessage.message (str(boost::format("Regel %d: EOF bij het lezen de totalen") %
                              EricFgetsGetLineCount (file)));
        EricFgetsClose (file);
        fclose (file);
        return 1;
    }
    if (sscanf (BitmapFileName, "%d%d",
                &mBaanInfo->RegelArray[RegelaarNummer].TotaalAfstand,
                &mBaanInfo->RegelArray[RegelaarNummer].TotaalTicken) != 2)
    {
        mBaanInfo->RegelArray[RegelaarNummer].TotaalAfstand = 0;
        mBaanInfo->RegelArray[RegelaarNummer].TotaalTicken = 0;
    }

    EricFgetsClose (file);
    fclose (file);
    mBaanInfo->RegelArray[RegelaarNummer].Gebruikt = 1;

    mBaanTreinen.baanAddTrein (mBaanInfo->RegelArray[RegelaarNummer].locSoort,
                               mBaanInfo->RegelArray[RegelaarNummer].locNaam,
                               RegelaarNummer,
                               mBaanInfo->RegelArray[RegelaarNummer].bitmapName);


    mBaanInfo->RegelArray[RegelaarNummer].td = mTd.tdCreate (mBaanInfo->RegelArray[RegelaarNummer].locNaam);
    return 0;

}

void
BaanDoc::baanDocPlaatsRegelaar (int RegelaarNummer, bool show)
{
    /* regel de plaatsing van het window van de regelaar */
    mBaanInfo->RegelArray[RegelaarNummer].Show(show);
    mTelefoonConnectie.SetActiveRegelaar(RegelaarNummer,
                                                  mBaanInfo->RegelArray[RegelaarNummer].Richting,
                                                  mBaanInfo->RegelArray[RegelaarNummer].getGewensteSnelheid());
}


void
BaanDoc::baanDocRegelaarOpen (const char *filename)
{
    int i;
    mKopRichtingDialoog.SetKopBlok(1);
    mKopRichtingDialoog.SetLengte(10);
    mKopRichtingDialoog.SetKopBlokMee(false);
    mKopRichtingDialoog.SetVooruitTerug(true);
    mKopRichtingDialoog.SetErrorText(" ");

    for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
    {
        if (mBaanInfo->RegelArray[i].Gebruikt == 0)
        {
            int TreinAccept;

            TreinAccept = 0;
            do
            {
                int blok;

                if (mKopRichtingDialoog.RunDialogOk())
                {

                    blok = mKopRichtingDialoog.GetKopBlok();
                    // controller of het gegeven kopblok ook wel bestaat
                    if ((blok < 0) || (blok >= MAX_NOBLOKS) ||
                            (mBaanInfo->BlokPointer[blok].BlokIONummer == -1))
                    {
                        mKopRichtingDialoog.SetErrorText("Sorry geen bestaand blok");
                    }
                    else
                    {
                        mBaanInfo->RegelaarAanvraag.Lengte = mKopRichtingDialoog.GetLengte();
                        mBaanInfo->RegelaarAanvraag.BlokNummer = blok;
                        mBaanInfo->RegelaarAanvraag.Richting =
                                (int) mKopRichtingDialoog.GetVoorUitTerug();
                        mBaanInfo->RegelaarAanvraag.NieuweRegelaar = 1;
                        mBaanInfo->RegelaarAanvraag.RegelaarNummer = i;
                        mBaanInfo->RegelaarAanvraag.NeemKopBlokMee = (int)mKopRichtingDialoog.GetKopBlokMee();
                        mBaanInfo->RegelaarAanvraag.Aanvraag = 1;

                        baanSemaphoreDown (mBaanInfo->RegelaarAanvraag.semWacht);

                        TreinAccept = mBaanInfo->RegelaarAanvraag.AanvraagAccept;
                        if (TreinAccept == 0)
                        {
                            mKopRichtingDialoog.SetErrorText("Niet alle blokken vrij!");
                        }
                        else
                        {
                            mBaanInfo->RegelArray[i].Richting =
                                    (int) mKopRichtingDialoog.GetVoorUitTerug();
                            mBaanInfo->RegelArray[i].pKopBlok =
                                    &mBaanInfo->BlokPointer[blok];
                            mBaanInfo->RegelArray[i].Lengte = mKopRichtingDialoog.GetLengte();
                        }
                    }
                }
                else
                    return;
            }
            while (TreinAccept == 0);

            mBaanInfo->RegelArray[i].m_pRegelAanvraag =
                    &(mBaanInfo->RegelaarAanvraag);

            sprintf (mBaanInfo->RegelArray[i].RegelaarFileName,
                     "%s", filename);

            baanDocInitRegelaar (i, 1);
            mMainWindowDrawing.RedrawAll();

            /* break this loop */
            i = MAX_AANTAL_REGELAARS;
        }
    }
}


void BaanDoc::baanDocHerlaadProgramma (int regelaar)
{
    if (mBaanInfo->RegelArray[regelaar].Gebruikt)
    {
        if (mBaanInfo->RegelArray[regelaar].herlaadProgramma)
        {
            // laad het programma opnieuw
            // CLAIMS worden door de unload verwijderd
            regelProg[regelaar].unload ();
            if (mBaanInfo->RegelArray[regelaar].programmaNaam[0] != 0)
            {
                // we moeten dus ook het programma naam veranderen
                strcpy (regelProg[regelaar].programmaNaam,
                        mBaanInfo->RegelArray[regelaar].programmaNaam);
            }
            regelProg[regelaar].Init (globaalProg.GetGlobalArray ());
            regelProg[regelaar].executeProgram (INIT, regelaar);
            mBaanInfo->RegelArray[regelaar].herlaadProgramma = 0;
        }
    }
}


void
BaanDoc::baanDocExecuteProgram (int regelaar, int eventType, int event)
{

    if (regelaar == -1)
    {
        // de globale regelaar
        mTd.tdStart (tdglobal);
        globaalProg.executeProgram (eventType, event);
        mTd.tdStop (tdglobal);
    }
    else
    {
        if (mBaanInfo->RegelArray[regelaar].programRunning)
        {
            mTd.tdStart (mBaanInfo->RegelArray[regelaar].td);
            regelProg[regelaar].executeProgram (eventType, event);
            mTd.tdStop (mBaanInfo->RegelArray[regelaar].td);
        }
    }
}

void
BaanDoc::baanDocStopProgram (int regelaar)
{
    // programma wordt gestopt dus hef claims op
    // als we weer gestart dan zou je dezelfde claim
    // nogmaals kunnen doen
    regelProg[regelaar].verwijderAanvragen ();
    // Voor het geval dat de trein voor een stop staat met een
    mBaanInfo->RegelArray[regelaar].zetGewensteSnelheid (1);
    regelProg[regelaar].blokStop (1, VERWIJDER_ALLE_STOPS);

}


void
BaanDoc::baanDocIniFile ()
{

    FILE *file;

    globaalProg.executeProgram (INIT, -1);

    // Hardware is in de lucht of simulatie
    // lees de ini file binnen en  restore de oude situtatie
    file = fopen (iniFileName, "rb");
    if (file != NULL)
    {
        int spoelen;
        int i;
        int version;

        char Array[MAX_FILENAME + 1];

        if (EricFgets (Array, MAX_FILENAME, file) == NULL)
        {
            mErrorPrint.errorPrint ("Regel %d: EOF Fout in lezen van het versie nummer",
                        EricFgetsGetLineCount (file));
            return;
        }

        sscanf (Array, "%d", &version);


        if (EricFgets (Array, MAX_FILENAME, file) == NULL)
        {
            mErrorPrint.errorPrint
                    ("Regel %d: EOF Fout in lezen aantal blokken en spoelen",
                     EricFgetsGetLineCount (file));
            return;
        }


        sscanf (Array, "%d", &spoelen);
        if (mBaanInfo->AantalSpoelen != spoelen)
        {
            mErrorPrint.errorPrint
                    ("Warning: Aantal spoelen verschillend\nIniFile wordt toch geladen");
        }

        /* het aantal blokken en spoelen is het zelfde met de vorige run */
        for (i = 0; i < spoelen; i++)
        {
            int Stand, VorigeStand;
            int type, wisselNummer;
            float adres;

            if (EricFgets (Array, MAX_FILENAME, file) == NULL)
            {
                mErrorPrint.errorPrint
                        ("Regel %d: EOF Fout in lezen van het versie nummer",
                         EricFgetsGetLineCount (file));
                return;
            }
            sscanf (Array, "%d%f%d\n", &type, &adres, &VorigeStand);
            wisselNummer = mWissels.ZoekWisselNummer (mBaanInfo->IOBits, adres);
            // check of dit nog steeds een zelfde type wissel is
            if ((wisselNummer < 0)
                    || (mBaanInfo->IOBits[wisselNummer].get()->Type != type)  // niet dezelfde wissel type dus er is iets gewijzigd
                    || (mBaanInfo->IOBits[wisselNummer].get()->Type == DRAAISCHIJF)) // voor draaischijf doen we geen recover
            {
                continue;         // deze wissel is niet meer dezelfde wissel niet gebruiken dus
            }

            if (mWissels.WisselStand (&Stand, wisselNummer))
            {
                /* hier klopt iets niet we verlaten het saven */
                i = mBaanInfo->AantalSpoelen;
            }
            else
            {
                if (Stand != 0)
                {
                    IOAanvraag_t IOAanvraag;
                    if (VorigeStand != Stand)
                    {
                        IOAanvraag.stand = VorigeStand;
                    }
                    else
                    {
                        IOAanvraag.stand = IOAANVRAAG_REFRESH;
                    }
                    IOAanvraag.IONummer = wisselNummer;
                    mWissels.Aanvraag (&IOAanvraag);

                    mBaanWT.ericSleep (20);       // 20ms max 5 wissels per cycle
                }
            }
        }
        // wacht zo lang totdat we op zijn minst een cycle
        // gedaan hebben in de work threat
        // Hte komt namelijk voor dat de worktreat wordt
        // onderbroken en deze procedure verder kan
        // Dit heeft tot gevolg dat de regelaar geinitializeerd
        // worden terwijl de wissels nog niet gezet zijn
        mBaanWT.ericSleep (200);

        /*
       ** Initializeer de regelaars en de blokken
       **/

        for (i = 0; i < MAX_AANTAL_REGELAARS; i++)
        {
            mBaanInfo->RegelArray[i].m_pRegelAanvraag =
                    &(mBaanInfo->RegelaarAanvraag);
            // ik probeer een string te lezen als dit niet lukt
            // stoppen we
            if (EricFgets
                    (mBaanInfo->RegelArray[i].RegelaarFileName, 256,
                     file) != NULL)
            {
                if (EricFgets (Array, MAX_FILENAME, file) == NULL)
                {
                    mErrorPrint.errorPrint
                            ("Regel %d: EOF Fout in lezen richting en Lengte regelaar %d",
                             EricFgetsGetLineCount (file), i);
                    return;
                }

                if (sscanf (Array, "%d%d",
                            &mBaanInfo->RegelArray[i].Richting,
                            &mBaanInfo->RegelArray[i].Lengte) == 2)
                {
                    int BlokType, BlokNummer, aantalBlokken;
                    int richting, tegenRichting;
                    BlokPointer_t *pBlok;
                    if (EricFgets (Array, MAX_FILENAME, file) == NULL)
                    {
                        mErrorPrint.errorPrint
                                ("Regel %d: EOF Fout in het lezen van BlokType BlokNummer aantalBlokken regelaar %d",
                                 EricFgetsGetLineCount (file), i);
                        return;
                    }

                    if (sscanf
                            (Array, "%d%d%d", &BlokType, &BlokNummer,
                             &aantalBlokken) == 3)
                    {
                        if (BlokType == WISSEL_BLOK)
                        {
                            // Voor een kruising kan je op een WISSEL_BLOK staan
                            mBaanInfo->RegelArray[i].pKopBlok =
                                    &mBaanInfo->IOBits[BlokNummer].get()->StopBlokPointer[0];
                        }
                        else
                        {
                            mBaanInfo->RegelArray[i].pKopBlok =
                                    &mBaanInfo->BlokPointer[BlokNummer];
                        }
                        if (baanDocInitRegelaar (i, 0) == 0)
                        {
                            mBaanInfo->RegelArray[i].Gebruikt = 1;
                        }
                        if (mBaanInfo->RegelArray[i].Gebruikt)
                        {
                            // beleg de blokken
                            pBlok = mBaanInfo->RegelArray[i].pKopBlok;
                            richting = mBaanInfo->RegelArray[i].Richting,
                                    tegenRichting = (richting + 1) & 1;
                            do
                            {
                                pBlok->pBlok->State = BLOK_VOORUIT + richting;
                                pBlok->pBlok->RegelaarNummer = i;
                                pBlok = pBlok->blokRicht[tegenRichting];
                                aantalBlokken -= 1;
                                if ((NULL == pBlok)
                                        || (pBlok->pBlok->State != BLOK_VRIJ))
                                {
                                    // stop ermee want we wijzen naar een eindblok
                                    aantalBlokken = 0;
                                }
                            }
                            while (aantalBlokken > 0);
                        }
                    }
                }
            }
            else
            {
                // EOF dus eruit
                i = MAX_AANTAL_REGELAARS;
            }
        }
        mMainWindowDrawing.RedrawAll();

        EricFgetsClose (file);
        fclose (file);
    }                           // if (file != NULL)
}




void
BaanDoc::baanDocAddBlok ()
{
    int nieuwBlok;
    baanDocSetWisselsDefault ();


    nieuwBlok = mBlok.BlokVindVrijBlok ();
    if (nieuwBlok)
    {
        mAddBlokDialoog.SetVrijBlok(nieuwBlok);
        if (mAddBlokDialoog.RunDialogOk())
        {
            // ok add het blok
            nieuwBlok = mAddBlokDialoog.GetVrijBlok();
            if (mBlok.BlokIsVrij (nieuwBlok))
            {
                mBaanInfo->BlokPointer[nieuwBlok].Lengte = 0;
                mBaanInfo->BlokPointer[nieuwBlok].BlokIONummer = nieuwBlok;
                mBaanInfo->Blok[nieuwBlok].XCoord = 10;
                mBaanInfo->Blok[nieuwBlok].YCoord = 10;
                mBaanInfo->AantalBlokken += 1;
                mBaanInfo->selectedBlok = nieuwBlok;
                mBaanInfo->selectedWissel = NULL;
                mBaanInfo->selectedWisselPoint = -1;
                mMainWindowDrawing.RedrawAll();

            }
            else
            {
                mMessage.message (str(boost::format("Blok %d is niet vrij") % nieuwBlok));
            }
        }

    }
    else
    {
        mMessage.message ("Alle blokken bezet vergroot MAX_NOBLOKS");
    }
}


void
BaanDoc::baanDocNieuwIo ()
{
    baanDocSetWisselsDefault ();
    mNieuwIODialoog.SetAdres(1.0);
    mNieuwIODialoog.SetBlok(1);
    mNieuwIODialoog.SetTypeIndex(0);

    std::vector<std::string> labels;
    labels.push_back("ontkoppelaar");
    labels.push_back("standaard wissel");
    labels.push_back("kruising");
    labels.push_back("engelse wissel");
    labels.push_back("drieweg wissel");
    labels.push_back("Lamp");
    labels.push_back("Draaischijf");
    mNieuwIODialoog.SetTypeLabels(labels);

    if (mNieuwIODialoog.RunDialogOk())
    {
        int kopBlok;
        float adres;
        int type;

        kopBlok = mNieuwIODialoog.GetBlok();
        if (mBlok.BlokIsBlokNummer (kopBlok) == 0)
        {
            kopBlok = 0;
        }
        if (mBlok.BlokIsVrij (kopBlok))
        {
            kopBlok = 0;
        }
        adres = mNieuwIODialoog.GetAdres();
        type = mNieuwIODialoog.GetTypeIndex() + 1;
        mWissels.WisselNieuw (adres, type, kopBlok);
        mMainWindowDrawing.RedrawAll();
    }
}
