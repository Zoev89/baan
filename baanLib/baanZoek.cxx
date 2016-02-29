#include "baanZoek.h"
#include "stdio.h"
#include <boost/lexical_cast.hpp>
#include <limits>
#include "baanLib.h"

BaanZoek::BaanZoek(BaanInfo &baanInfo, IWissels &wissels) :
    m_baanInfo(baanInfo),
    mWissels(wissels),
    m_penalty(50)
{
}

bool BaanZoek::isBlokAanwezigInResultNaarDeWisselToe(EersteWisselResult res, int huidigBlok, int gewenstBlok, int regelaarNummer, int richting)
{
    std::vector<int> blokList = m_baanInfo.GetBlokList(res.wisselNummer, res.knoopPuntNummer);
    
    for(vector<int>::reverse_iterator it = blokList.rbegin(), end = blokList.rend(); it != end; ++it)
    {
        if (huidigBlok == *it)
        {
            huidigBlok = -1;
        }
        if (huidigBlok == -1)
        {
            if (!isBlokVrij(*it,regelaarNummer, richting))
                return false;
            if (*it == gewenstBlok)
                return true;
        }
    }
    return false;
}

bool BaanZoek::isBlokAanwezigInResultVanDeWisselAf(EersteWisselResult res, int gewenstBlok, int regelaarNummer, int richting)
{
    std::vector<int> blokList = m_baanInfo.GetBlokList(res.wisselNummer, res.knoopPuntNummer);
    
    for(vector<int>::iterator it = blokList.begin(), end = blokList.end(); it != end; ++it)
    {
        if (!isBlokVrij(*it,regelaarNummer, richting))
            return false;
        if (*it == gewenstBlok)
            return true;
    }
    return false;
}

int BaanZoek::BlokListLengte(int initialKnoopPunt, int wisselNummer, int knoopPuntNummer, int richting)
{
    std::vector<int> blokList = m_baanInfo.GetBlokList(wisselNummer, knoopPuntNummer);
    int lengte = mWissels.wisselLengte(initialKnoopPunt, wisselNummer, knoopPuntNummer);
    
    for(vector<int>::iterator it = blokList.begin(), end = blokList.end(); it != end; ++it)
    {
        lengte += abs(m_baanInfo.GetBaanInfo()->BlokPointer[*it].Lengte);
    }
    lengte = (m_baanInfo.GetBaanInfo()->IOBits[wisselNummer].get()->routeKnoopPunt[knoopPuntNummer].richtingVoorkeur & (richting+1)) ?
      lengte : 3*lengte;
    return lengte;
}

bool BaanZoek::isBlokVrijInBlokList(int wisselNummer, int knoopPuntNummer, int regelaarNummer, int richting)
{
    std::vector<int> blokList = m_baanInfo.GetBlokList(wisselNummer, knoopPuntNummer);
    for(vector<int>::iterator it = blokList.begin(), end = blokList.end(); it != end; ++it)
    {
        if (!isBlokVrij(*it,regelaarNummer, richting))
            return false;
    }
    return true;
}


class ZoekElement
{
public:    
    ZoekElement(EersteWisselResult res, int vorigeIndex,int lengte) : 
        m_wisselResult(res),
        m_vorigeIndex(vorigeIndex),
        m_lengte(lengte)
    {
    };
    EersteWisselResult m_wisselResult;
    int m_vorigeIndex;
    int m_lengte;
};

class ZoekResultaat
{
public:    
    ZoekResultaat(int wisselNummer, int knoopPunt, int diepte, int index, int lengte) : 
        m_wisselNummer(wisselNummer),
        m_knoopPunt(knoopPunt),
        m_diepte(diepte),
        m_index(index),
        m_lengte(lengte)
    {
    };
    int m_wisselNummer;
    int m_knoopPunt;
    int m_diepte;
    int m_index;
    int m_lengte;
};
#define AANTAL_ROUTE_AANSLUITINGEN 4
class WelkeWisselGebruikt
{
    struct WelkeWisselGebruiktData
    {
        bool gebruikt;
        int lengte;
    };
public:    
    WelkeWisselGebruikt(int aantalspoelen)
    {
        WelkeWisselGebruiktData data;
        data.gebruikt = false;
        data.lengte = 0;
        m_boekHouding = vector<WelkeWisselGebruiktData>(aantalspoelen*AANTAL_ROUTE_AANSLUITINGEN, data);
    };
    void KnoopPuntBezocht(int wisselNummer,int knoopPuntNummer, int lengte)
    {
        WelkeWisselGebruiktData data;
        data.gebruikt = true;
        data.lengte = lengte;
        m_boekHouding[wisselNummer*AANTAL_ROUTE_AANSLUITINGEN + knoopPuntNummer] = data;
    };
    bool IsKnoopPuntBezocht(int wisselNummer,int knoopPuntNummer)
    {
        return m_boekHouding[wisselNummer*AANTAL_ROUTE_AANSLUITINGEN + knoopPuntNummer].gebruikt;
    };
    int KnoopPuntLengte(int wisselNummer,int knoopPuntNummer)
    {
        return m_boekHouding[wisselNummer*AANTAL_ROUTE_AANSLUITINGEN + knoopPuntNummer].lengte;
    };
private:
    vector<WelkeWisselGebruiktData> m_boekHouding;
};

bool BaanZoek::isBlokVrij(int blok, int regelaarNummer, int richting)
{
    Blok_t * pBlok = &m_baanInfo.GetBaanInfo()->Blok[blok];
    bool ret = (((pBlok->State & BLOK_BEZET_MASK )== BLOK_VRIJ) || 
        ((regelaarNummer >= 0) && (pBlok->RegelaarNummer == regelaarNummer)));
    ret &= (pBlok->blokBelegt == 0) || 
        ((pBlok->blokBelegt == 1) && (pBlok->blokBelegtRegelaar == regelaarNummer));
    if ((regelaarNummer >= 0) && (pBlok->Bovenleiding == 0) && m_baanInfo.GetBaanInfo()->RegelArray[regelaarNummer].ELoc)
        ret = false;
    if ((pBlok->richtingVoorkeur & (richting+1)) == 0)
        ret = false;
    return ret;
}

std::vector<BaanZoekResultaat> BaanZoek::pad(int huidigBlok, int richting,int gewenstBlok, int regelaarNummer)
{
    std::vector<BaanZoekResultaat> zoekPad;
    std::vector<ZoekResultaat> zoekResultaat;
    int richt = richting-RICHTING_VOORUIT;
    std::vector<std::vector<ZoekElement> > zoekBoom;
    WelkeWisselGebruikt knoopPuntGebruikt(m_baanInfo.GetBaanInfo()->AantalSpoelen);
    
    if (!isBlokVrij(huidigBlok, regelaarNummer, richt))
    {
        return zoekPad;
    }
    if (!isBlokVrij(gewenstBlok, regelaarNummer, richt))
    {
        return zoekPad;
    }

    if (huidigBlok == gewenstBlok)
    {
        // veiliger om een nul pad niet gevonden te zeggen als je je zelf aanroept
        // er wordt dan uiteindelijk niets belegd en er is dan een kans dat er toch
        // gereden gaat worden zonder aankomst
        return zoekPad;
    }
    
    EersteWisselResult res = mWissels.VindEersteWissel(huidigBlok,richt);
    if (res.isWissel())
    {
        std::vector<ZoekElement> nieuwBlad;
        nieuwBlad.push_back(ZoekElement(res, -1, 0));
        zoekBoom.push_back(nieuwBlad);
        int maxLengte = INT_MAX;
        int huidigeDiepte = 0;
        bool gevonden=isBlokAanwezigInResultNaarDeWisselToe(zoekBoom[huidigeDiepte].at(0).m_wisselResult, huidigBlok, gewenstBlok, regelaarNummer, richt);
        int gewenst = gevonden ? gewenstBlok: -1;
        while ((gevonden==false)  && (huidigeDiepte < 20) && (zoekBoom[huidigeDiepte].size() != 0))
        {
            //printf("Diepte %d\n",huidigeDiepte);
            int size = zoekBoom[huidigeDiepte].size();
            int nieuweDiepte = huidigeDiepte+1;
            {
                std::vector<ZoekElement> nieuwBlad;
                zoekBoom.push_back(nieuwBlad);
            }
            for(int i=0;i<size;i++)
            {
                ZoekElement huidigElement = zoekBoom[huidigeDiepte].at(i);
                int wisselNummer = huidigElement.m_wisselResult.wisselNummer;
                IOBits_t * pWissel = m_baanInfo.GetBaanInfo()->IOBits[wisselNummer].get();
                int knoopPunt = huidigElement.m_wisselResult.knoopPuntNummer;
            
                for(vector<int>::iterator it = pWissel->routeKnoopPunt[knoopPunt].aansluitingen.begin(),
                end = pWissel->routeKnoopPunt[knoopPunt].aansluitingen.end(); it != end; ++it)
                {
                    if (isBlokAanwezigInResultVanDeWisselAf(EersteWisselResult(wisselNummer,*it), gewenstBlok,regelaarNummer,richt))
                    {
                        // een oplossing gevonden maar we gaan wel verder zoeken
                        zoekResultaat.push_back(ZoekResultaat(wisselNummer, *it, huidigeDiepte, i,huidigElement.m_lengte));
                        if (maxLengte > huidigElement.m_lengte)
                        {
                            maxLengte = huidigElement.m_lengte;
                        }
                        //gevonden = true;
                        //printf("gevonden %d\n",maxLengte);
                    }
                    else
                    {
                        if (pWissel->routeKnoopPunt[*it].wisselNummer != -1)
                        {
                            // geen dood spoor dus volgende wissel toevoegen als
                            // Het knooppunt nog niet bezocht is
                            // De lengte tot nu toe kleiner is dan de bezochte lengte
                            // En alle blokken moeten vrij zijn anders wordt die ook niet toegevoegd
                            int nieuweLengte = BlokListLengte(knoopPunt, wisselNummer,*it, richt)+huidigElement.m_lengte;
                            if ((((knoopPuntGebruikt.IsKnoopPuntBezocht(wisselNummer, *it) == false) ||
                                (knoopPuntGebruikt.KnoopPuntLengte(wisselNummer, *it)>(nieuweLengte+m_penalty))) &&
                                (nieuweLengte < maxLengte)) && isBlokVrijInBlokList(wisselNummer,*it, regelaarNummer, richt))
                            {
                                //printf("Insert %d.%2d %d\n",pWissel->hardwareAdres,pWissel->hardwareBit,*it);
                                knoopPuntGebruikt.KnoopPuntBezocht(wisselNummer, *it, nieuweLengte);
                                EersteWisselResult wissel(pWissel->routeKnoopPunt[*it].wisselNummer,
                                     pWissel->routeKnoopPunt[*it].knoopPunt);
                                ZoekElement element(wissel, i, nieuweLengte);
                                zoekBoom[nieuweDiepte].push_back(element);
                            }
                        }
                    }
                }
            }
            huidigeDiepte += 1;
        }
        if ((gevonden) || (zoekResultaat.size() > 0))
        {
            if (zoekResultaat.size() > 0)
            {
                // er zijn een of meerdere zoek resultaten kies de kleinste lengte
                int indexStart = 0;
                int lengte = zoekResultaat[0].m_lengte;
                for(std::size_t i=1;i<zoekResultaat.size();++i)
                {
                    // voor de volgende resultaten gebruik ik een penalty
                    if ((zoekResultaat[i].m_lengte + m_penalty) < lengte)
                    {
                        indexStart = i;
                        lengte = zoekResultaat[i].m_lengte;
                    }
                }
                
                ZoekResultaat result = zoekResultaat[indexStart];
                //printf("Lengte %d\n",result.m_lengte);
                EersteWisselResult wissel(result.m_wisselNummer,result.m_knoopPunt);
                std::vector<int> blokList = m_baanInfo.GetBlokList(result.m_wisselNummer,result.m_knoopPunt);
                bool insert = false;
                for(vector<int>::reverse_iterator rit = blokList.rbegin(), end = blokList.rend(); rit != end; ++rit)
                {
                    int value = *rit;
                    if (value == gewenstBlok)
                        insert = true;
                    if (insert)
                    {
                        zoekPad.insert(zoekPad.begin(), BaanZoekResultaat(value));
                    }
                }
                int vorigKnoopPunt = result.m_knoopPunt;
                int vorigeIndex = result.m_index;
                for(int diepte = result.m_diepte; diepte>=0;--diepte)
                {
                    ZoekElement element = zoekBoom[diepte].at(vorigeIndex);
                    zoekPad.insert(zoekPad.begin(),BaanZoekResultaat(element.m_wisselResult.wisselNummer,
                        mWissels.wisselStand(element.m_wisselResult.wisselNummer,
                            vorigKnoopPunt,element.m_wisselResult.knoopPuntNummer)));
                    if (diepte > 0)
                    {   // laatste blok heeft geen wissel om terug tekijken
                        std::vector<int> blokList = m_baanInfo.GetBlokList(
                            element.m_wisselResult.wisselNummer, 
                            element.m_wisselResult.knoopPuntNummer);
                        for(vector<int>::iterator rit = blokList.begin(), end = blokList.end(); rit != end; ++rit)
                        {
                            zoekPad.insert(zoekPad.begin(),BaanZoekResultaat(*rit));
                        }
                        vorigKnoopPunt = m_baanInfo.GetBaanInfo()->IOBits[element.m_wisselResult.wisselNummer].get()->
                            routeKnoopPunt[element.m_wisselResult.knoopPuntNummer].knoopPunt;
                    }
                    vorigeIndex = element.m_vorigeIndex;
                }
            }
            // Nu nog de blokken tot aan de wissel
            // pasop gewenst heeft een gewenstBlok waarde als het gewenst blok voor de wissel is gevonden
            // anders is die -1
            std::vector<int> blokList = m_baanInfo.GetBlokList(res.wisselNummer, res.knoopPuntNummer);
            for (std::size_t offset=0;offset<blokList.size();offset++)
            {
                if (blokList[offset]==gewenst)
                {
                    gewenst = -1;
                }
                if (gewenst == -1)
                {
                    zoekPad.insert(zoekPad.begin(), BaanZoekResultaat(blokList[offset]));
                }
                if (blokList[offset]==huidigBlok)
                {
                    gewenst = 0;
                }
            }
        }
    }
    else
    {
        // Er is geen wissel meer in deze richting maar misschien wel het gewenste blok
        BlokPointer_t * hBlok = m_baanInfo.GetBlokPointer(huidigBlok);
        BlokPointer_t * nextBlok = hBlok->blokRicht[richt];
        while ((nextBlok->BlokIONummer != gewenstBlok) &&
              (nextBlok->BlokIONummer != huidigBlok))
        {
            nextBlok = nextBlok->blokRicht[richt];
        }
        if (nextBlok->BlokIONummer == gewenstBlok)
        {
            zoekPad.push_back(huidigBlok);
            BlokPointer_t * hBlok = m_baanInfo.GetBlokPointer(huidigBlok);
            BlokPointer_t * nextBlok = hBlok->blokRicht[richt];
            while ((nextBlok->BlokIONummer != gewenstBlok) &&
                  (nextBlok->BlokIONummer != huidigBlok))
            {
                zoekPad.push_back(nextBlok->BlokIONummer);
                nextBlok = nextBlok->blokRicht[richt];
            }
            zoekPad.push_back(nextBlok->BlokIONummer);
        }
    }
/*    int size = 0;
    for(vector<std::vector<ZoekElement> >::iterator it = zoekBoom.begin(), end = zoekBoom.end(); it != end; ++it)
    {
        size += it->size();
    }
    printf("totalSize %d\n",size);*/
    return zoekPad;
}

