#ifndef IOVERZICHT_H
#define IOVERZICHT_H
#include <string>
#include <vector>
#include "baanTypes.h"

class IOverzicht
{
public:
    virtual std::vector<std::string> GetOverzicht(BaanInfo_t *pInfo)=0;
};

#endif // IOVERZICHT_H
