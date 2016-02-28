#if !defined(_BLOK_H_)
#define _BLOK_H_

#include "baanTypes.h"
#include "IBlok.h"
#include "IMessage.h"
#include "IMainWindowDrawing.h"
#include "ITd.h"

class Blok: public IBlok
{
public:
    Blok(IMessage& msg, IMainWindowDrawing& mainWindowDrawing, ITd& td, BaanInfo_t *baanInfo);
    // IBlok
    virtual char *BlokNaam(char *string,BlokPointer_t *blok);
    virtual void BlokDisplay(int blokNummer, int regelaar,
                             int actie);
    virtual void BlokInsert(BlokPointer_t *newBlok);
    virtual void BlokDelete(BlokPointer_t *delBlok);
    virtual void BlokEndPointDelete(BlokPointer_t * delBlok,int richting);
    virtual void BlokEndPointInsert(BlokPointer_t * insBlok,int richting);
    virtual void BlokPrint(BlokPointer_t * blok);
    virtual int BlokIsBlokNummer(int blokNummer); // return 0 if ok else 1
    virtual int BlokIsVrij(int blokNummer);  // return 1 if vrij
    virtual int BlokVindVrijBlok(); // return blok nummer, 0=error
private:
    IMessage& mMessage;
    IMainWindowDrawing& mMainWindowDrawing;
    ITd& mTd;
    BaanInfo_t *mBaanInfo;
    int lastVrij;        // wordt gebruikt om de gebruiker niet steeds hetzelfde blok te geven voor BlokVindVrijBlok
    int tdview;
    int tdOnBlokDisplay;
};


#endif // !defined(_BLOK_H_)
