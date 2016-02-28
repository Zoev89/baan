#include "baanTypes.h"
#include "baanWT.h"
#include <string.h>
#include "foutStatistiek.h"

#define AANTALIO 40

FoutStatistiek::FoutStatistiek()
{

}

std::vector<std::string>
FoutStatistiek::GetFoutStatistiek (BaanInfo_t * pInfo)
{
    int i, p;
    std::vector<std::string> returnVector;

    returnVector.push_back("BLOK\taantal fouten");

    for (i = 0; i < MAX_NOBLOKS; i++)
    {

        if (pInfo->BlokPointer[i].BlokIONummer != -1)
        {
            int total;
            total = 0;
            // dit blok wordt gebruikt
            for (p = 0; p < MAX_AANTAL_REGELAARS; p++)
            {
                if (pInfo->RegelArray[p].Gebruikt)
                {
                    total += pInfo->RegelArray[p].aantalFouten[i];
                }
            }
            if (total)
            {
                char inhoud[100];
                sprintf (inhoud, "%d\t%d", i, total);
                returnVector.push_back(inhoud);
            }
        }
    }
    return returnVector;
}
