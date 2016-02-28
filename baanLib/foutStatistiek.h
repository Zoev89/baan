#ifndef FOUTSTATISTIEK_H
#define FOUTSTATISTIEK_H

#include "baanTypes.h"
#include <vector>
#include <string>

class FoutStatistiek
{
public:
    FoutStatistiek();
    std::vector<std::string> GetFoutStatistiek (BaanInfo_t * pInfo);

private:
};

#endif
