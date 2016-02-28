#ifndef IWISSELS_H
#define IWISSELS_H
#include "IIoBits.h"
#include <memory>

class EersteWisselResult
{
public:
    EersteWisselResult(int _wisselNummer, int _knoopPuntNummer) :
        wisselNummer(_wisselNummer),
        knoopPuntNummer(_knoopPuntNummer)
    {

    };
    bool isWissel()
    {
        return wisselNummer != -1;
    };

    int wisselNummer;
    int knoopPuntNummer;
};

struct IOAanvraag_t;

class IWissels
{
public:
    ~IWissels() = default;

    // Bijna de IWissel interface op TestIOTimer,Aanvraag na en Init andere volgoorde van parameters
    virtual int Init (int WisselNummer, char *Input, FILE * file)=0;
    virtual void InitRoutering (int WisselNummer)=0;
    virtual void ZoekOngeinitializeerdeEindBlokken(int WisselNummer)=0;
    virtual void Display (int WisselNummer, bool updateBitmap) = 0;
    virtual int Aanvraag (IOAanvraag_t * aanvraag)=0;
    virtual void String(int WisselNummer, char *string)=0;
    virtual void NieuwXY (int WisselNummer,int selectionX, int selectionY, int deltaX, int deltaY)=0;
    virtual bool InitDialoog (int WisselNummer)=0;
    virtual void DialoogOk (int WisselNummer)=0;

    virtual void TestIOTimers ()=0;

    virtual int CheckWissel (int WisselNummer)=0;
    virtual int WisselStand (int *Stand, int WisselNummer)=0;
    virtual int ZoekWisselNummer (std::vector<std::unique_ptr<IOBits_t>> &wissels, float adres)=0;
    virtual BlokPointer_t *wisselKrijgPointer (int BlokType,
                       float adres)=0;
    virtual void WisselVeranderBlok (int WisselNummer, BlokPointer_t *oudBlok,  BlokPointer_t *nieuwBlok)=0;
    virtual void WisselNieuw(float adres, int type, int kopBlok)=0;
    virtual void WisselDelete(int WisselNummer)=0;
    virtual int wisselStand(int wisselNummer, int knp1, int knp2)=0;
    virtual int wisselLengte(int initialKnoopPunt, int wisselNummer, int knoopPuntNummer)=0;
    virtual EersteWisselResult VindEersteWissel(int blokNummer, int richting)=0;
    virtual int WisselIndex(IOBits_t * pWissel) = 0;
    virtual void CreateNewIO(int type) = 0;

    // lokaal spul wat alleen door wissels word gebruikt
    virtual void VolgKnoopPuntReverse(IOBits_t * pWissel, BlokPointer_t * pBlok, int knoopPuntIndex,
    IOBits_t * eigenWissel=NULL)=0;
    virtual void VolgKnoopPunt(IOBits_t * pWissel, BlokPointer_t * pBlok, int knoopPuntIndex,
    IOBits_t * eigenWissel=NULL, int richting = 0)=0;
};

#endif // IWISSELS_H
