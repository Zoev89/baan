#ifndef UIBAANLIBCREATOR_H
#define UIBAANLIBCREATOR_H
#include <memory>
#include "baanLib.h"
#include "QWidget"

#include "UIMessage.h"
#include "UIMainScreenControls.h"
#include "UIMainWindowDrawing.h"
#include "UITreinenDialoog.h"
#include "QtBlokInstDialoog.h"
#include "QtWisselDialoog.h"
#include "UILampInstDialoog.h"
#include "QtKopRichtingDialoog.h"
#include "QtNieuwIODialoog.h"
#include "UIAddBlokDialoog.h"
#include "UIRegelaarInstellingenDialoog.h"
#include "UIRegelaarViewUpdates.h"
#include "ThreadSleep.h"
#include "hardwareCom.h"

class UIBaanLibCreator
{
public:
    UIBaanLibCreator(QWidget * parent);

    UIMessage message;
    UIMainScreenControls mainScreenControls;
    std::unique_ptr<UIMainWindowDrawing> mainWindowDrawing;
    UITreinenDialoog treinenDialoog;
    QtBlokInstDialoog blokInst;
    QtWisselDialoog wisselDialoog;
    UILampInstDialoog lampInstDialoog;
    QtKopRichtingDialoog kopRichtingDialoog;
    QtNieuwIODialoog nieuwIODialoog;
    UIAddBlokDialoog addBlok;
    std::unique_ptr<UIRegelaarViewUpdates> regelaarViewUpdates;
    UIRegelaarInstellingenDialoog regelaarInstellingenDialoog;
    ThreadSleep mThreadSleep; // moet voor mBaanLib zitten want anders is de distuctie volgoorde niet goed
    HardwareCom mHardwareHoog;
    HardwareCom mHardwareLaag;
    std::unique_ptr<BaanLib> mBaanLib;

};

#endif // UIBAANLIBCREATOR_H
