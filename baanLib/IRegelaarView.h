#ifndef IREGELAARVIEW_H
#define IREGELAARVIEW_H
#include <string>

class IRegelaarView
{
public:
    // snelheid 0..63
    virtual void SetSnelheid(int snelheid) = 0;
    virtual int GetSnelheid() = 0;
    // richting 0=vooruit 1=achteruit
    virtual void SetRichting(int vt) = 0;
    virtual int GetRichting() = 0;
    virtual void SetIgnoreStop(bool ignore) = 0;
    virtual bool GetIgnoreStop() = 0;
    virtual void RunProgramClicked(bool run) = 0;
    virtual void VerwijderClicked() = 0;
    virtual void PropertiesClicked() = 0;
    virtual std::string GetBitmapFileName() = 0;
};

#endif // IREGELAARVIEW_H
