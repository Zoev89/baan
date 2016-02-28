#ifndef ILAMPINSTDIALOOG_H
#define ILAMPINSTDIALOOG_H

class ILampInstDialoog
{
public:
    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetAanTijd(int tijd) = 0;
    virtual int  GetAanTijd() = 0;
    virtual void  SetUitTijd(int tijd) = 0;
    virtual int  GetUitTijd() = 0;
};
#endif // ILAMPINSTDIALOOG_H
