#include "UIBaanLibCreator.h"

UIBaanLibCreator::UIBaanLibCreator(QWidget * parent)
{
    mainWindowDrawing.reset(new UIMainWindowDrawing(parent));
    regelaarViewUpdates.reset(new UIRegelaarViewUpdates(parent));
    mBaanLib.reset(new BaanLib(message, mainScreenControls, *mainWindowDrawing.get(), treinenDialoog, blokInst, *regelaarViewUpdates.get(), regelaarInstellingenDialoog, wisselDialoog, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog,
                               addBlok, mThreadSleep));

}

