#ifndef BAANLIB_H
#define BAANLIB_H

#include "IMessage.h"
#include "baanDoc.h"
#include "baanWT.h"
#include "baanTreinen.h"
#include "blok.h"
#include "wissel.h"
#include "IMainScreenControls.h"
#include "IMainWindowDrawing.h"
#include "ITreinenDialoog.h"
#include "baanOverzicht.h"
#include "ioOverzicht.h"
#include "baanMessage.h"
#include "baanView.h"
#include "IBlokInst.h"
#include "foutStatistiek.h"
#include "IWisselDialoog.h"
#include "IKopRichtingDialoog.h"
#include "INieuwIODialoog.h"
#include "IAddBlokDialoog.h"
#include "IRegelaarViewUpdates.h"
#include "IRegelaarInstellingenDialoog.h"
#include "TelefoonConnectie.h"
#include "td.h"
#include "errorPrint.h"

class BaanLib: public IView
{
public:
    BaanLib(IMessage& message, IMainScreenControls& mainControls, IMainWindowDrawing& mainWindowDrawing, ITreinenDialoog& treinenDialoog, IBlokInst& blokInst, IRegelaarViewUpdates& regelaarViewUpdates,
            IRegelaarInstellingenDialoog& regelaarInstellingenDialoog, IWisselDialoog& wisselDialoog, ILampInstDialoog &lampInstDialoog, IKopRichtingDialoog& kopRichtingDialoog, INieuwIODialoog& nieuwIODialoog,
            IAddBlokDialoog& addBlokDialoog, IThreadSleep& threadSleep);
    ~BaanLib();

    // Interface to main document
    void BaanClose();
    void BaanOpen(const std::string& inputFilename, bool editMode);
    void BaanSchrijfBlkFile();
    void BaanRegelaarOpen(const std::string& regelaarFilename);
    std::vector<string> BaanFoutStatistiek();
    void TdDump();
    std::vector<std::string> GetIoOverzicht();
    std::vector<std::string> GetBaanOverzicht();
    void BaanAddBlok();
    void BaanDocNieuwIo();
    void BaanSetWisselsDefault ();
    int BaanCheckDatabase();
    void TreinenDialogActivateRegelaar(int regelaarNummer);
    std::string baanDocGetBitmapname();


    // IView
    virtual void Draw();
    virtual bool HandleMouseEvents (MouseEventType type, MouseButtonType button,int x, int y);
    virtual bool HandleKeyBoardEvent(KeyBoardEventType key);

    // IRegelaarView afgeleide
    virtual void SetSnelheid(int regelaar, int snelheid);
    virtual int GetSnelheid(int regelaar);
    // richting 0=vooruit 1=achteruit
    virtual void SetRichting(int regelaar, int vt);
    virtual int GetRichting(int regelaar);
    virtual void SetIgnoreStop(int regelaar, bool ignore);
    virtual bool GetIgnoreStop(int regelaar);
    virtual void RunProgramClicked(int regelaar, bool run);
    virtual void VerwijderClicked(int regelaar);
    virtual void PropertiesClicked(int regelaar);
    virtual std::string GetBitmapFileName(int regelaar);


private:
    BaanInfo_t mBaanInfo;
    Td mTd;
    IMessage& mMessage;
    ErrorPrint mErrorPrint;
    BaanWT mBaanWT;
    BaanDoc mBaanDoc;
    BaanTreinen mBaanTreinen;
    Blok mBlok;
    BaanOverzicht mBaanOverzicht;
    IoOverzicht mIoOverzicht;
    BaanMessage mBaanMessage;
    BaanView mBaanView;
    FoutStatistiek mFoutStatistiek;
    Wissels mWissels;
    TelefoonConnectie mTelefoonConnectie;
};

#endif // BAANLIB_H
