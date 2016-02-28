#ifndef UILAMPINSTDIALOOG_H
#define UILAMPINSTDIALOOG_H
#include "ILampInstDialoog.h"

class UILampInstDialoog : public ILampInstDialoog
{
public:
    UILampInstDialoog();

    virtual bool RunDialogOk(); // returns true when ok is pressed

    virtual void  SetAanTijd(int tijd);
    virtual int  GetAanTijd();
    virtual void  SetUitTijd(int tijd);
    virtual int  GetUitTijd();
};

#endif // UILAMPINSTDIALOOG_H
