#pragma once
#include <vector>
#include "baanWT.h"
#include "wissel.h"
#include "IWissels.h"

class BaanZoekResultaat
{
public:
    BaanZoekResultaat(int blokNummer) :
        m_blokNummer(blokNummer),
        m_wisselNummer(-1),
        m_stand(-1)
    {
    };
    BaanZoekResultaat(int wisselNummer, int stand) :
        m_blokNummer(-1),
        m_wisselNummer(wisselNummer),
        m_stand(stand)
    {
    };
    bool IsBlok()
    {
        return m_blokNummer != -1;
    }
    bool IsWissel()
    {
        return m_wisselNummer != -1;
    }
    int m_blokNummer;
    int m_wisselNummer;
    int m_stand;
};

inline bool operator==(const BaanZoekResultaat &lhr,const BaanZoekResultaat &rhr)
{
    return (lhr.m_blokNummer == rhr.m_blokNummer) &&
        (lhr.m_wisselNummer == rhr.m_wisselNummer) && 
        (lhr.m_stand == rhr.m_stand);
};

class BaanZoek
{
public:
    friend class BaanZoekTest_BlokListLengte_Test;
    BaanZoek(BaanInfo &baanInfo, IWissels& wissels);
    std::vector<BaanZoekResultaat> pad(int huidigBlok, int richting,int gewenstBlok, int regelaarNummer = -1);
private:
    bool isBlokAanwezigInResultNaarDeWisselToe(EersteWisselResult res, int huidigBlok, int gewenstBlok, int regelaarNummer, int richting);
    bool isBlokAanwezigInResultVanDeWisselAf(EersteWisselResult res, int gewenstBlok, int regelaarNummer, int richting);
    int BlokListLengte(int initialKnoopPunt, int wisselNummer, int knoopPuntNummer, int richting);
    bool isBlokVrij(int blok, int regelaarNummer, int richting);
    bool isBlokVrijInBlokList(int wisselNummer, int knoopPuntNummer, int regelaarNummer , int richting);
    BaanInfo &m_baanInfo;
    IWissels& mWissels;
    const int m_penalty;
};
