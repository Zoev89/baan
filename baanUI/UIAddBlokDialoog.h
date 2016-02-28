#ifndef UIADDBLOKDIALOOG_H
#define UIADDBLOKDIALOOG_H
#include "IAddBlokDialoog.h"

class UIAddBlokDialoog : public IAddBlokDialoog
{
public:
    UIAddBlokDialoog();

    virtual bool RunDialogOk(); // returns true when ok is pressed

    virtual void  SetVrijBlok(int blok);
    virtual int  GetVrijBlok();


};

#endif // UIADDBLOKDIALOOG_H
