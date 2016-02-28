#ifndef _baanView_h_
#define _baanView_h_

#include "baanQueue.h"
#include "IMessage.h"
#include "IBaanDoc.h"
#include "IBlok.h"
#include "IBlokInst.h"
#include "IWissels.h"
#include "IMainWindowDrawing.h"
#include "IView.h"
#include "IViewInternal.h"
#include "ITd.h"
#include "baanTypes.h"

class BaanView : public IView, public IViewInternal
{
public:
    BaanView(IMessage& msg, IBaanDoc& baanDoc, IBlok& blok, IBlokInst& blokInst, IWissels& wissels
             , IMainWindowDrawing &mainWindowDrawing, ITd& td, BaanInfo_t *baanInfo);

    // IView
    virtual void Draw();
    virtual bool HandleMouseEvents (MouseEventType type, MouseButtonType button,int x, int y);
    virtual bool HandleKeyBoardEvent(KeyBoardEventType key);

    //IViewInternal
    virtual void Update(int type, int value1, int value2, int value3);
private:
    void baanViewBlokXY(int blok,int x,int y);
    void baanViewVierkantDisplay (int xy, int lengteHoogte, IMainWindowDrawing::BaanColor color);
    IMessage& mMessage;
    IBaanDoc& mBaanDoc;
    IBlok& mBlok;
    IBlokInst& mBlokInst;
    IWissels& mWissels;
    IMainWindowDrawing &mMainWindowDrawing;
    ITd& mTd;
    BaanInfo_t *mBaanInfo;

    int tdview;
    int tdDraw;
    int tdOnBlokEvent;
    int tdOnTimerVerlopen;
    int tdOnInternalTimerVerlopen;

};

#endif // _baanView_h_

