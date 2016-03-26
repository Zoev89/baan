#ifndef BAANLIBSETUP_H
#define BAANLIBSETUP_H
#include "AllMocks.h"
#include "baanLib.h"

#include "baanTypes.h"
#include "baanDoc.h"
#include "baanZoek.h"
#include "wissel.h"
#include "programma.h"
#include "baanWT.h"
#include "blok.h"


class InitObjects
{
public:
    InitObjects(BaanInfo_t *baanInfo) :
        baanWT(message,baanMessage,wissels, td, baanInfo),
        blok(message,mainWindowDrawing, td, baanInfo),
        wissels(message, blok, mainWindowDrawing, baanMessage, wisselDialoog, baanInfo, threadSleep),
        baanDoc(message, baanWT, baanTreinen, blok, wissels, mainScreenControls, mainWindowDrawing,regelaarViewUpdates,
                baanMessage, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog, addBlokDialoog, regelaarInstellingenDialoog, telefoonConnectie, td, errorPrint, baanInfo)
    {
        baanWT.InitWorkThread();
    }
    testing::NiceMock<ITdMock> td; // moet bovenaan staan zodat die geinitializeerd is!

    BaanWT baanWT;
    Blok blok;
    Wissels wissels;
    BaanDoc baanDoc;

// alles public zodat ik in de testen objecten kan initializeren
    IMessageMock message;
    IBaanTreinenMock baanTreinen;
    IBaanMessageMock baanMessage;
    IViewMock baanView;
    IMainScreenControlsMock mainScreenControls;
    IMainWindowDrawingMock mainWindowDrawing;
    ITreinenDialoogMock treinenDialoog;
    IBlokInstMock blokInst;
    IWisselDialoogMock wisselDialoog;
    ILampInstDialoogMock lampInstDialoog;
    IKopRichtingDialoogMock kopRichtingDialoog;
    INieuwIODialoogMock nieuwIODialoog;
    IAddBlokDialoogMock addBlokDialoog;
    IRegelaarInstellingenDialoogMock regelaarInstellingenDialoog;
    IRegelaarViewUpdatesMock regelaarViewUpdates;
    ITelefoonConnectieMock telefoonConnectie;
    IErrorPrintMock errorPrint;
    IThreadSleepMock threadSleep;
};

#endif // BAANLIBSETUP_H
