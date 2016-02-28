#ifndef IMAINSCREENCONTROLS_H
#define IMAINSCREENCONTROLS_H
#include <string>

class IMainScreenControls
{
public:
    virtual ~IMainScreenControls() {}
    virtual void mainWindowLabel(const std::string& msg) = 0;
    virtual void mainWindowOpenActivate() = 0;
    virtual void mainWindowOpenDeactivate() = 0;
    virtual void mainWindowCloseActivate() = 0;
    virtual void mainWindowCloseDeactivate() = 0;
    virtual void mainWindowTreinActivate() = 0;
    virtual void mainWindowTreinDeactivate() = 0;
    virtual void mainWindowRedraw() = 0;

};


#endif // IMAINSCREENCONTROLS_H
