#ifndef IREGELAARVIEWUPDATES_H
#define IREGELAARVIEWUPDATES_H
#include <string>

class IRegelaarViewUpdates
{
public:
    ~IRegelaarViewUpdates() = default;

    virtual void NieuweRegelaar(int regelaarNummer, const std::string& bitmapFileName) = 0;
    virtual void SnelheidProgress(int regelaarNummer, int snelheid) = 0;
    virtual void Meldingen(int regelaarNummer, const std::string& meldingText) = 0;
    virtual void NewSnelheid(int regelaarNummer, int snelheid) = 0; // wordt aangeroepen vanuit de programma code
    virtual void RichtingVerandering(int regelaarNummer, int richting) = 0;
    virtual void IgnoreStop(int regelaarNummer, bool ignore)= 0 ;
    virtual void ShowRegelaar(int regelaarNummer, bool show) = 0;
};

#endif // IREGELAARVIEWUPDATES_H
