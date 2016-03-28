#include "hardwareCom.h"

HardwareCom::HardwareCom ()
{
  // breng leeg conditie aan
  kop = 0;
  staart = 0;
  maxLengte = 0;
}

int
HardwareCom::aantalItems ()
{
  int max;
  max = kop - staart;
  if (max < 0)
    {
      max += MAX_AANTAL_AANVRAGEN;
    }
  return max;
}

int
HardwareCom::nieuwItem (const hardwareArray_t &data)
{
  int nieuwKop;
  int max;

  //if (mBaanInfo->editMode)
  //   return 0;                   // hardware communicatie niet mogelijk

  if (data.adres == -1)
  {
      return 0;
  }

  nieuwKop = kop + 1;
  if (nieuwKop == MAX_AANTAL_AANVRAGEN)
    {
      nieuwKop = 0;
    }
  if (nieuwKop == staart)
    {
      // de vol conditie
      // note verhoog kop alleen in de andere case
      return 1;
    }
  array[kop] = data;
  kop = nieuwKop;
  max = kop - staart;
  if (max < 0)
    {
      max += MAX_AANTAL_AANVRAGEN;
    }
  if (max > maxLengte)
    {
      maxLengte = max;
    }
  return 0;
}

int
HardwareCom::krijgItem (hardwareArray_t &data)
{
  int nieuwStaart;

  if (kop == staart)
    {
      // de array is leeg
      return 1;
    }
  nieuwStaart = staart + 1;
  if (nieuwStaart == MAX_AANTAL_AANVRAGEN)
    {
      nieuwStaart = 0;
    }
  data = array[staart];
  staart = nieuwStaart;
  return 0;
}
