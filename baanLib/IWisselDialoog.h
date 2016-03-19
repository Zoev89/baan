#ifndef IWISSELDIALOOG_H
#define IWISSELDIALOOG_H
#include <string>
#include <vector>

struct DraaiSchijfAansluiting
{
    int aansluitingNummer; // [0..47]
    int blok; //-1 voor blok wat onder de draaischijf zit [-1..1024]
    bool gndFirst;
    bool richtingVooruit;
};

class IWisselDialoog
{
public:
    ~IWisselDialoog() = default;

    virtual void ClearAllFields() = 0; // Omdat de dialoog voor alle wissels word gebruikt moet de context afhankelijk zijn
    virtual bool RunDialogOk() = 0; // returns true when ok is pressed

    virtual void  SetAdres(float adres) = 0; // xxx.yy notation
    virtual float GetAdres() = 0;

    virtual void  SetUitleg(const std::string& uitleg) = 0;
    virtual std::string GetUitleg() = 0;

    virtual void  SetHwAan(int hwAan) = 0;
    virtual int   GetHwAan() = 0;
    virtual void  SetHwAanToolTip(const std::string& tooltip) = 0;

    virtual void  SetHwTot(int hwTot) = 0;
    virtual int   GetHwTot() = 0;
    virtual void  SetHwTotToolTip(const std::string& tooltip) = 0;

    virtual void  SetTypeIndex(int index) = 0;
    virtual int   GetTypeIndex() = 0;
    virtual void  SetTypeLabels(std::vector<std::string> labels) = 0;
    virtual std::vector<std::string> GetTypeLabels() = 0;

    virtual void  SetAanTijd(int ticks) = 0;
    virtual int   GetAanTijd() = 0;
    virtual void  SetAanTijdToolTip(const std::string& tooltip) = 0;

    virtual void  SetUitTijd(int ticks) = 0;
    virtual int   GetUitTijd() = 0;
    virtual void  SetUitTijdToolTip(const std::string& tooltip) = 0;

    virtual void  SetHoogte(int hoogte) = 0;
    virtual int   GetHoogte() = 0;
    virtual void  SetHoogteToolTip(const std::string& tooltip) = 0;

    virtual void  SetBreedte(int breedte) = 0;
    virtual int   GetBreedte() = 0;
    virtual void  SetBreedteToolTip(const std::string& tooltip) = 0;

    // wissels
    virtual void  SetAansluiting(int index, const std::string& aansluiting) = 0;
    virtual std::string GetAansluiting(int index) = 0;
    virtual void  SetAansluitingToolTip(int index, const std::string& tooltip) = 0;

    virtual void  SetLengte(int index, int lengte) = 0;
    virtual int   GetLengte(int index) = 0;
    virtual void  SetLengteToolTip(int index, const std::string& tooltip) = 0;

    virtual void  SetMaxSnelheid(int index, int snelheid) = 0;
    virtual int   GetMaxSnelheid(int index) = 0;
    virtual void  SetMaxSnelheidToolTip(int index, const std::string& tooltip) = 0;

    // draaischijf spul
    virtual void SetDraaiAansluitingen(std::vector<DraaiSchijfAansluiting> aansluitingen) = 0;
    virtual std::vector<DraaiSchijfAansluiting> GetDraaiAansluitingen() = 0;

};


#endif // IWISSELDIALOOG_H
