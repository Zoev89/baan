#if !defined(_WISSEL_H_)
#define _WISSEL_H_

// zie de C file voor de beschrijving
#define WISSEL_ERR_INVALID_ADRES 1
#define WISSEL_ERR_NIET_ALLES_AANWEZIG 2
#define WISSEL_ERR_INVALID_TYPE 3
#define WISSEL_ERR_MESSAGE_AL_GEGEVEN 4

#define SQR(x) ((x)*(x))
#include "baanTypes.h"
#include "IWissels.h"
#include "IBlok.h"
#include "IMessage.h"
#include "IMainWindowDrawing.h"
#include "IBaanMessage.h"
#include "IWisselDialoog.h"

class Wissels: public IWissels
{
public:
    Wissels(IMessage& msg, IBlok& blok, IMainWindowDrawing &mainWindowDrawing, IBaanMessage& baanMessage
            , IWisselDialoog& wisselDialoog, BaanInfo_t *baanInfo);
    // IWissels
    virtual int Init(int WisselNummer, const char *Input, std::function<std::string()> newInput) override;
    virtual void InitRoutering (int WisselNummer) override;
    virtual void ZoekOngeinitializeerdeEindBlokken(int WisselNummer) override;
    virtual void Display (int WisselNummer, bool bitmap) override;
    virtual int Aanvraag (IOAanvraag_t * aanvraag) override;
    virtual void String(int WisselNummer, char *string) override;
    virtual void NieuwXY (int WisselNummer,int selectionX, int selectionY, int deltaX, int deltaY) override;
    virtual bool InitDialoog (int WisselNummer) override;
    virtual void DialoogOk (int WisselNummer) override;

    virtual void TestIOTimers () override;

    virtual int CheckWissel (int WisselNummer) override;
    virtual int WisselStand (int *Stand, int WisselNummer) override;
    virtual int ZoekWisselNummer (std::vector<std::unique_ptr<IOBits_t>> &wissels, float adres) override;
    virtual BlokPointer_t *wisselKrijgPointer (int BlokType,
                       float adres) override;
    virtual void WisselVeranderBlok (int WisselNummer, BlokPointer_t *oudBlok,  BlokPointer_t *nieuwBlok) override;
    virtual void WisselNieuw(float adres, int type, int kopBlok) override;
    virtual void WisselDelete(int WisselNummer) override;
    virtual int wisselStand(int wisselNummer, int knp1, int knp2) override;
    virtual int wisselLengte(int initialKnoopPunt, int wisselNummer, int knoopPuntNummer) override;
    virtual EersteWisselResult VindEersteWissel(int blokNummer, int richting) override;
    virtual int WisselIndex(IOBits_t * pWissel) override;
    virtual void CreateNewIO(int type) override;

    // lokaal spul wat alleen door wissels word gebruikt
    virtual void VolgKnoopPuntReverse(IOBits_t * pWissel, BlokPointer_t * pBlok, int knoopPuntIndex,
    IOBits_t * eigenWissel=NULL);
    virtual void VolgKnoopPunt(IOBits_t * pWissel, BlokPointer_t * pBlok, int knoopPuntIndex,
    IOBits_t * eigenWissel=NULL, int richting = 0);

private:
    int IsBlokEenWisselNummer(BlokPointer_t * blok,IOBits_t * eigenWissel=NULL);
    bool KanBlokWordenToegevoegd(BlokPointer_t * pBlok,IOBits_t * eigenWissel=NULL);
    int WisselKnoopPuntNummer(int nummer, BlokPointer_t * pBlok);
    int CheckWisselBlok (BlokPointer_t * pBlok, int Richting);
    void WisselVerwijder (int WisselNummer);
    void WisselCopyBlokListReverse(std::vector<int>& dst, std::vector<int>& src);
    void CreateNewIO(int type, int kopBlok);

    IMessage& mMessage;
    IBlok& mBlok;
    IMainWindowDrawing &mMainWindowDrawing;
    IBaanMessage& mBaanMessage;
    IWisselDialoog& mWisselDialoog;
    BaanInfo_t *mBaanInfo;
};

#endif // !defined(_BAANWT_H_)
