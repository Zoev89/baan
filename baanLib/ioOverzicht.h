#ifndef IOOVERZICHT_H
#define IOOVERZICHT_H

#include "baanTypes.h"

#include "IOverzicht.h"

class IoOverzicht: public IOverzicht
{
public:
    IoOverzicht();
    virtual std::vector<std::string> GetOverzicht(BaanInfo_t *pInfo);
};

void ioOverzicht(BaanInfo_t *pInfo);
void ioOverzichtDestroy();

#endif
