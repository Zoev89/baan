#include "baanLib.h"
#include "baanTypes.h"
#include "td.h"
#include "foutStatistiek.h"
#include "baanOverzicht.h"
#include "ioOverzicht.h"


BaanLib::BaanLib(IMessage& message, IMainScreenControls& mainControls, IMainWindowDrawing& mainWindowDrawing, ITreinenDialoog &treinenDialoog, IBlokInst& blokInst, IRegelaarViewUpdates& regelaarViewUpdates,
                 IRegelaarInstellingenDialoog& regelaarInstellingenDialoog, IWisselDialoog &wisselDialoog, ILampInstDialoog& lampInstDialoog, IKopRichtingDialoog& kopRichtingDialoog, INieuwIODialoog &nieuwIODialoog,
                 IAddBlokDialoog &addBlokDialoog, IThreadSleep& threadSleep, IHardwareCom &hardwareHoog, IHardwareCom & hardwareLaag)
    : mBaanInfo(hardwareHoog,hardwareLaag)
    , mTd(10000)
    , mMessage(message)
    , mErrorPrint(mBaanInfo.tickTimer)
    , mBaanWT(message,mBaanMessage, mWissels, mTd, &mBaanInfo)
    , mBaanDoc(message, mBaanWT, mBaanTreinen, mBlok, mWissels, mainControls, mainWindowDrawing, regelaarViewUpdates, mBaanMessage, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog, addBlokDialoog, regelaarInstellingenDialoog,
             mTelefoonConnectie, mTd, mErrorPrint, &mBaanInfo)
    , mBaanTreinen(mBaanDoc, treinenDialoog)
    , mBlok(message, mainWindowDrawing, mTd, &mBaanInfo)
    , mBaanMessage(mBaanView, mTd, mBaanInfo.RegelArray)
    , mBaanView(message, mBaanDoc,mBlok, blokInst, mWissels, mainWindowDrawing, mTd, &mBaanInfo)
    , mWissels(message,mBlok,mainWindowDrawing,mBaanMessage, wisselDialoog, &mBaanInfo, threadSleep)
    , mTelefoonConnectie(mBaanInfo.RegelArray)
{
    mMessage.message("Ik ben er");
}

BaanLib::~BaanLib()
{
    mBaanDoc.baanDocClose();
}

void BaanLib::BaanClose()
{
    mBaanDoc.baanDocClose();
}

void BaanLib::BaanOpen(const string &inputFilename, bool editMode)
{
    mBaanInfo.editMode = (editMode) ? 1:0;
    mBaanDoc.baanDocOpen(inputFilename.c_str());
}

std::string BaanLib::baanDocGetBitmapname()
{
    return mBaanDoc.baanDocGetBitmapname();
}

void BaanLib::BaanSchrijfBlkFile()
{
    mBaanDoc.baanDocSchrijfBlkFile();
}

void BaanLib::BaanRegelaarOpen(const string &regelaarFilename)
{
    mBaanDoc.baanDocRegelaarOpen(regelaarFilename.c_str());
}

std::vector<std::string>  BaanLib::BaanFoutStatistiek()
{
    return mFoutStatistiek.GetFoutStatistiek(&mBaanInfo);
}

void BaanLib::TdDump()
{
    mTd.tdDump();
}

std::vector<std::string> BaanLib::GetIoOverzicht()
{
    return mIoOverzicht.GetOverzicht(&mBaanInfo);
}

std::vector<std::string> BaanLib::GetBaanOverzicht()
{
    return mBaanOverzicht.GetOverzicht(&mBaanInfo);
}

void BaanLib::BaanAddBlok()
{
    mBaanDoc.baanDocAddBlok();
}

void BaanLib::BaanDocNieuwIo()
{
    mBaanDoc.baanDocNieuwIo();
}

void BaanLib::BaanSetWisselsDefault()
{
    mBaanDoc.baanDocSetWisselsDefault();
}

int BaanLib::BaanCheckDatabase()
{
    return mBaanDoc.baanDocCheckDatabase();
}

void BaanLib::TreinenDialogActivateRegelaar(int regelaarNummer)
{
    mBaanTreinen.baanTreinenActivateRegelaar(regelaarNummer);
}

void BaanLib::Draw()
{
    mBaanView.Draw();
}

bool BaanLib::HandleMouseEvents (MouseEventType type, MouseButtonType button,int x, int y)
{
    return mBaanView.HandleMouseEvents(type,button,x,y);
}

bool BaanLib::HandleKeyBoardEvent(KeyBoardEventType key)
{
    return mBaanView.HandleKeyBoardEvent(key);
}

void BaanLib::SetSnelheid(int regelaar, int snelheid)
{
    mBaanInfo.RegelArray[regelaar].SetSnelheid(snelheid);
}

int BaanLib::GetSnelheid(int regelaar)
{
    return mBaanInfo.RegelArray[regelaar].GetSnelheid();

}

void BaanLib::SetRichting(int regelaar, int vt)
{
    mBaanInfo.RegelArray[regelaar].SetRichting(vt);

}

int BaanLib::GetRichting(int regelaar)
{
    return mBaanInfo.RegelArray[regelaar].GetRichting();

}

void BaanLib::SetIgnoreStop(int regelaar, bool ignore)
{
    mBaanInfo.RegelArray[regelaar].SetIgnoreStop(ignore);

}

bool BaanLib::GetIgnoreStop(int regelaar)
{
    return mBaanInfo.RegelArray[regelaar].GetIgnoreStop();
}

void BaanLib::RunProgramClicked(int regelaar, bool run)
{
    mBaanInfo.RegelArray[regelaar].RunProgramClicked(run);
}

void BaanLib::VerwijderClicked(int regelaar)
{
    mBaanInfo.RegelArray[regelaar].VerwijderClicked();
}

void BaanLib::PropertiesClicked(int regelaar)
{
    mBaanInfo.RegelArray[regelaar].PropertiesClicked();
}

std::string BaanLib::GetBitmapFileName(int regelaar)
{
    return mBaanInfo.RegelArray[regelaar].GetBitmapFileName();
}


