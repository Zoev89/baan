#ifndef UIMAINSCREENCONTROLS_H
#define UIMAINSCREENCONTROLS_H
#include "IMainScreenControls.h"

class UIMainScreenControls : public IMainScreenControls
{
public:
    UIMainScreenControls();

    virtual void mainWindowLabel(const std::string& msg);
    virtual void mainWindowOpenActivate();
    virtual void mainWindowOpenDeactivate();
    virtual void mainWindowCloseActivate();
    virtual void mainWindowCloseDeactivate();
    virtual void mainWindowTreinActivate();
    virtual void mainWindowTreinDeactivate();
    virtual void mainWindowRedraw();

};

#endif // UIMAINSCREENCONTROLS_H
