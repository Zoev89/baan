#ifndef BAANTREINEN_H
#define BAANTREINEN_H
#include "IBaanTreinen.h"
#include "IBaanDoc.h"
#include "ITreinenDialoog.h"


class BaanTreinen: public IBaanTreinen
{
public:
    BaanTreinen(IBaanDoc& doc, ITreinenDialoog &treinenDialoog);

    // IBaanTreinen
    virtual void baanCreateTreinen (int x, int y, int width, int height);
    virtual void baanAddTrein (char *soort, char *name, int regelaarNummer, const std::string& imageName);
    virtual void baanDestroyTreinen();
    virtual void baanVerwijderTrein (int regelaarNummer);
    virtual void baanTreinenActivateRegelaar (int regelaarNummer);

private:
    IBaanDoc& mBaanDoc;
    ITreinenDialoog& mTreinenDialoog;
};

#endif



