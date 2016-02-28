#ifndef UINIEUWIODIALOOG_H
#define UINIEUWIODIALOOG_H
#include "INieuwIODialoog.h"

class UINieuwIODialoog : public INieuwIODialoog
{
public:
    UINieuwIODialoog();

    virtual bool RunDialogOk(); // returns true when ok is pressed

    virtual void  SetAdres(float adres);
    virtual float GetAdres();
    virtual void  SetBlok(int blok);
    virtual int   GetBlok();

    virtual void  SetTypeIndex(int index);
    virtual int   GetTypeIndex();
    virtual void  SetTypeLabels(std::vector<std::string> labels);
    virtual std::vector<std::string> GetTypeLabels();
};

#endif // UINIEUWIODIALOOG_H
