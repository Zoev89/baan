#ifndef IADDBLOKDIALOOG_H
#define IADDBLOKDIALOOG_H

class IAddBlokDialoog
{
public:
    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetVrijBlok(int blok) = 0;
    virtual int  GetVrijBlok() = 0;
};

#endif // IADDBLOKDIALOOG_H
