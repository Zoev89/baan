#ifndef BAANOVERZICHT_H
#define BAANOVERZICHT_H

#include "IOverzicht.h"

class BaanOverzicht: public IOverzicht
{
public:
    BaanOverzicht();
    virtual std::vector<std::string> GetOverzicht(BaanInfo_t *pInfo);
};
#endif
