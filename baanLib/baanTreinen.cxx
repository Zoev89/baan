#include "baanTreinen.h"
#include "baan.h"
#include <string.h>


BaanTreinen::BaanTreinen(IBaanDoc& doc, ITreinenDialoog &treinenDialoog):
    mBaanDoc(doc),
    mTreinenDialoog(treinenDialoog)
{
}

void BaanTreinen::baanTreinenActivateRegelaar (int regelaar)
{
    if ((regelaar >= 0) && (regelaar < MAX_AANTAL_REGELAARS))
    {
      mBaanDoc.baanDocPlaatsRegelaar (regelaar, true);
    }
}


void BaanTreinen::baanCreateTreinen (int x, int y, int width, int height)
{
    mTreinenDialoog.CreateTreinen(x,y,width,height);
}

void BaanTreinen::baanDestroyTreinen ()
{
    mTreinenDialoog.DestroyTreinen();
}




void BaanTreinen::baanAddTrein (char *soort, char *name, int regelaarNummer,
              const std::string& imageName)
{
    mTreinenDialoog.AddTrein(soort,name,regelaarNummer,imageName);
}

void BaanTreinen::baanVerwijderTrein (int regelaarNummer)
{
    mTreinenDialoog.VerwijderTrein(regelaarNummer);
}
