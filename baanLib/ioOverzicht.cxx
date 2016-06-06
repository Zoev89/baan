#include "ioOverzicht.h"
#include "baanWT.h"

#define AANTALIO 40

IoOverzicht::IoOverzicht()
{

}

// De oude implementatie gebruikte de table met 17 velden waarbij de breedtes werden initializeerd op:
// int widths[] = {
//   40,
//   30, 30, 30, 30,
//   30, 30, 30, 30,
//   30, 30, 30, 30,
//   30, 30, 30, 30
// };
std::vector<std::string> IoOverzicht::GetOverzicht (BaanInfo_t * pInfo)
{
    int i, p;
    int ios[AANTALIO][17];

    memset (ios, 0, sizeof (ios));

    // we zoeken alle spoelen af en maken een tabel per io controller
    for (i = 0; i < pInfo->AantalSpoelen; i++)
    {
        int Type;
        IOBits_t *pWissel;

        pWissel = pInfo->IOBits[i].get();
        Type = pWissel->Type;
        for (p = 0; p < AANTALIO; p++)
        {
            if ((0 == ios[p][0]) || (pWissel->hardwareAdres == ios[p][0]))
            {
                // gevonden of een nieuwe entry
                ios[p][0] = pWissel->hardwareAdres;
                ios[p][pWissel->hardwareBit + 1] = pWissel->Type;
                if (DRIEWEG_WISSEL == Type)
                {
                    ios[p][pWissel->hardwareBit + 2] = pWissel->Type;
                }
                if (DRAAISCHIJF == Type)
                {
                    ios[p+1][0] = pWissel->hardwareAdres+1;
                    ios[p+2][0] = pWissel->hardwareAdres+2;
                    for(int index=1;index<17;index++)
                    {
                        ios[p][index] = pWissel->Type;
                        ios[p+1][index] = pWissel->Type;
                        ios[p+2][index] = pWissel->Type;
                    }

                }
                break;
            }
        }


    }
    // tabel opgebouwed nu het displayen
    std::vector<std::string> returnVector;

    returnVector.push_back(" IO\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15");

    for (p = 0; p < AANTALIO; p++)
    {
        char inhoud[100];
        if (0 == ios[p][0])
        {
            break;
        }
        else
        {
            sprintf (inhoud, "%d", ios[p][0]);
            for (i = 1; i < 17; i++)
            {
                const char *t;
                switch (ios[p][i])
                {
                case 0:
                    t = "vrij";
                    break;
                case 1:
                    t = "ont";
                    break;
                case 2:
                    t = "wis";
                    break;
                case 3:
                    t = "kruis";
                    break;
                case 4:
                    t = "eng";
                    break;
                case 5:
                    t = "drie";
                    break;
                case 6:
                    t = "lamp";
                    break;
                case 7:
                    t = "draai";
                    break;
                default:
                    t = "?";
                }
                sprintf (inhoud, "%s\t%s", inhoud, t);
            }
            returnVector.push_back(inhoud);
        }
    }
    return returnVector;
}
