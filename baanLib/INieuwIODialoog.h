#ifndef INIEUWIODIALOOG_H
#define INIEUWIODIALOOG_H
#include <string>
#include <vector>

class INieuwIODialoog
{
public:
    virtual ~INieuwIODialoog() {}

    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetAdres(float adres) = 0;
    virtual float GetAdres() = 0;
    virtual void  SetBlok(int blok) = 0;
    virtual int   GetBlok() = 0;

    virtual void  SetTypeIndex(int index) = 0;
    virtual int   GetTypeIndex() = 0;
    virtual void  SetTypeLabels(std::vector<std::string> labels) = 0;
    virtual std::vector<std::string> GetTypeLabels() = 0;
};



#endif // INIEUWIODIALOOG_H
