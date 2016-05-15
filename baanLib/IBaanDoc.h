#ifndef IBAANDOC_H
#define IBAANDOC_H

#include <string>

class IBaanDoc
{
public:
    virtual void baanDocLeftMouseButtonDown(int x,int y) = 0;
    virtual void baanDocRightMouseButtonDown(int x,int y) = 0;
    virtual void baanDocSetWisselsDefault() = 0;
    virtual void baanDocExecuteProgram (int regelaar, int eventType, int event) = 0;
    virtual void baanDocStopProgram(int regelaar) = 0;
    virtual void baanDocPlaatsRegelaar(int RegelaarNummer, bool show) = 0;
    virtual std::string baanDocGetBitmapname() = 0;
};

#endif // IBAANDOC_H
