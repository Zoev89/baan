#ifndef _baanDoc_h_
#define _baanDoc_h_
#include <stdio.h>

#include "IMessage.h"
#include "IBaanWT.h"
#include "IBaanDoc.h"
#include "IBlok.h"
#include "programma.h"
#include "IBaanTreinen.h"
#include "IMainScreenControls.h"
#include "IMainWindowDrawing.h"
#include "IBaanMessage.h"
#include "ILampInstDialoog.h"
#include "IKopRichtingDialoog.h"
#include "INieuwIODialoog.h"
#include "IAddBlokDialoog.h"
#include "IRegelaarInstellingenDialoog.h"
#include "IRegelaarViewUpdates.h"
#include "ITelefoonConnectie.h"
#include "ITd.h"
#include "IErrorPrint.h"

#include <vector>

class BaanDoc: public IBaanDoc
{
public:
    BaanDoc(IMessage& msg, IBaanWT& baanWT, IBaanTreinen &baanTreinen, IBlok& blok, IWissels& wissels, IMainScreenControls& mainScreenControls, IMainWindowDrawing &mainWindowDrawing, IRegelaarViewUpdates& regelaarViewUpdates,
            IBaanMessage& baanMessage, ILampInstDialoog& lampInstDialoog, IKopRichtingDialoog& kopRichtingDialoog, INieuwIODialoog& nieuwIODialoog, IAddBlokDialoog& addBlokDialoog,
            IRegelaarInstellingenDialoog& regelaarInstellingenDialoog,
            ITelefoonConnectie& telefoonConnectie, ITd& td, IErrorPrint &errorPrint, BaanInfo_t *baanInfo);

    // IBaanDoc
    virtual void baanDocLeftMouseButtonDown(int x,int y);
    virtual void baanDocRightMouseButtonDown(int x,int y);
    virtual void baanDocSetWisselsDefault();
    virtual void baanDocExecuteProgram (int regelaar, int eventType, int event);
    virtual void baanDocStopProgram(int regelaar);
    virtual void baanDocPlaatsRegelaar(int RegelaarNummer, bool show);
    virtual void baanDocHerlaadProgramma(int regelaar);
    virtual std::string baanDocGetBitmapname();

    // Nog niet in IBaanDoc later bekijken of dat moet
    void baanDocOpen(const char *doc);
    void baanDocClose();
    void baanDocRegelaarOpen(const char *filename);
    void baanDocAddBlok();
    void baanDocNieuwIo();
    int baanDocCheckDatabase();
    void baanDocSchrijfBlkFile ();

public: // for testing only! baanZoek
    FILE *baanDocFileOpen(const char *filename,const char *acces, char *dirName,char **name);
    int baanDocParseBlkFile (FILE * file);
private:
    int BaanCheckBlokNummer (int Nummer, int max, char *Array);
    void baanDocIniFile ();
    int baanDocInitRegelaar (int RegelaarNummer, int show);


private:
    IMessage& mMessage;
    IMainScreenControls& mMainControls;
    IBaanWT& mBaanWT;
    IBaanTreinen& mBaanTreinen;
    IBlok& mBlok;
    IWissels& mWissels;
    IMainWindowDrawing& mMainWindowDrawing;
    IBaanMessage& mBaanMessage;
    ILampInstDialoog& mLampInstDialoog;
    IKopRichtingDialoog& mKopRichtingDialoog;
    INieuwIODialoog& mNieuwIODialoog;
    IAddBlokDialoog& mAddBlokDialoog;
    ITelefoonConnectie& mTelefoonConnectie;
    ITd& mTd;
    IErrorPrint& mErrorPrint;
    BaanInfo_t *mBaanInfo;
    char iniFileName[MAX_FILENAME];
    char blkFileName[MAX_FILENAME];
    char iniBlkFileName[MAX_FILENAME];       // de orginele filename in de blk file
    char bitmapBlkFileName[MAX_FILENAME];    // de orginele filename in de blk file

    CProgramma globaalProg;
    std::vector<CProgramma> regelProg;
    int treinenX, treinenY, treinenWidth, treinenHeight;
    int regelaarTopX, regelaarTopY, regelaarBotX, regelaarBotY;

    int tdglobal;

};



#endif // _baanDoc_h_

