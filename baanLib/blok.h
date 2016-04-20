#if !defined(_BLOK_H_)
#define _BLOK_H_

#include "baanTypes.h"
#include "IBlok.h"
#include "IMessage.h"
#include "IMainWindowDrawing.h"
#include "ITd.h"
#include "IWissels.h"

class Blok: public IBlok
{
public:
    Blok(IMessage& msg, IMainWindowDrawing& mainWindowDrawing, ITd& td, BaanInfo_t *baanInfo, IWissels& wissels);
    virtual ~Blok() {}
    // IBlok
    virtual bool InitBlok(std::string blok) override;
    virtual char *BlokNaam(char *string,BlokPointer_t *blok) override;
    virtual void BlokDisplay(int blokNummer, int regelaar,
                             int actie) override;
    virtual void BlokInsert(BlokPointer_t *newBlok) override;
    virtual void BlokDelete(BlokPointer_t *delBlok) override;
    virtual void BlokEndPointDelete(BlokPointer_t * delBlok,int richting) override;
    virtual void BlokEndPointInsert(BlokPointer_t * insBlok,int richting) override;
    virtual void BlokPrint(BlokPointer_t * blok) override;
    virtual int BlokIsBlokNummer(int blokNummer) override; // return 0 if ok else 1
    virtual int BlokIsVrij(int blokNummer) override;  // return 1 if vrij
    virtual int BlokVindVrijBlok() override; // return blok nummer, 0=error
private:
    IMessage& mMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    ITd& mTd;
    BaanInfo_t *mBaanInfo;
    IWissels& mWissels;
    int lastVrij;        // wordt gebruikt om de gebruiker niet steeds hetzelfde blok te geven voor BlokVindVrijBlok
    int tdview;
    int tdOnBlokDisplay;
};


#endif // !defined(_BLOK_H_)
