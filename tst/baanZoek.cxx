#include "gtest/gtest.h"
#include "baanTypes.h"
//#include "baanWT.h"
#include "baanDoc.h"
#include "baanZoek.h"
#include "wissel.h"
#include "programma.h"
#include "baanWT.h"
#include "blok.h"

#include "AllMocks.h"

class InitObjects
{
public:
    InitObjects(BaanInfo_t *baanInfo) :
        baanWT(message,baanMessage,wissels, td, baanInfo),
        blok(message,mainWindowDrawing, td, baanInfo),
        wissels(message, blok, mainWindowDrawing, baanMessage, wisselDialoog, baanInfo),
        baanDoc(message, baanWT, baanTreinen, blok, wissels, mainScreenControls, mainWindowDrawing,regelaarViewUpdates,
                baanMessage, lampInstDialoog, kopRichtingDialoog, nieuwIODialoog, addBlokDialoog, regelaarInstellingenDialoog, telefoonConnectie, td, errorPrint, baanInfo)
    {

    }
    ITdMock td; // moet bovenaan staan zodat die geinitializeerd is!

    BaanWT baanWT;
    Blok blok;
    Wissels wissels;
    BaanDoc baanDoc;

// alles public zodat ik in de testen objecten kan initializeren
    IMessageMock message;
    IBaanTreinenMock baanTreinen;
    IBaanMessageMock baanMessage;
    IViewMock baanView;
    IMainScreenControlsMock mainScreenControls;
    IMainWindowDrawingMock mainWindowDrawing;
    ITreinenDialoogMock treinenDialoog;
    IBlokInstMock blokInst;
    IWisselDialoogMock wisselDialoog;
    ILampInstDialoogMock lampInstDialoog;
    IKopRichtingDialoogMock kopRichtingDialoog;
    INieuwIODialoogMock nieuwIODialoog;
    IAddBlokDialoogMock addBlokDialoog;
    IRegelaarInstellingenDialoogMock regelaarInstellingenDialoog;
    IRegelaarViewUpdatesMock regelaarViewUpdates;
    ITelefoonConnectieMock telefoonConnectie;
    IErrorPrintMock errorPrint;

};

static InitObjects *objects;
static BaanInfo_t* baanInfo = NULL;

void TestFrameWorkInitBaan()
{
    if (baanInfo == NULL)
    {
        baanInfo = new BaanInfo_t;
        objects =  new InitObjects(baanInfo); // Program Init fails want de objects zijn niet geinitializeerd bij mBaanInfo new hier boven

        {
            for (int i=0;i<MAX_AANTAL_REGELAARS;i++)
                baanInfo->RegelArray.push_back(Regelaar(objects->regelaarViewUpdates, objects->baanTreinen, objects->baanDoc, objects->regelaarInstellingenDialoog, objects->baanMessage,
                                                        objects->telefoonConnectie, objects->td));
        }

        std::string doc="/home/eric/trein/ezb/baan.blk";
        FILE * blkFile = objects->baanDoc.baanDocFileOpen (doc.c_str(), "rb", baanInfo->blkDir, &baanInfo->blkName);
        baanInfo->spoorInfo.LoadData("/home/eric/trein/ezb/baan");
        ASSERT_FALSE(objects->baanDoc.baanDocParseBlkFile (blkFile));
   }
}

class BaanZoekTest : public ::testing::Test {
 protected:
    virtual void SetUp() {
        TestFrameWorkInitBaan();
        mBaanInfo = baanInfo;
    }
    virtual void TearDown() 
    {
    }

public:
    void printZoekPad(std::vector<BaanZoekResultaat> &pad)
    {
        for(unsigned int i=0;i<pad.size();++i)
        {
            if (pad[i].IsBlok())
            {
                printf("Blok %d\n",pad[i].m_blokNummer);
            }
            if (pad[i].IsWissel())
            {
                printf("Wissel %d.%02d %d\n",mBaanInfo->IOBits[pad[i].m_wisselNummer].get()->hardwareAdres,
                    mBaanInfo->IOBits[pad[i].m_wisselNummer].get()->hardwareBit,
                    pad[i].m_stand);
            }
        }
    };
    void codeZoekPad(std::vector<BaanZoekResultaat> &pad)
    {
        printf("    std::vector<BaanZoekResultaat> expect;\n");
        
        for(unsigned int i=0;i<pad.size();++i)
        {
            if (pad[i].IsBlok())
            {
                printf("    expect.push_back(BaanZoekResultaat(%d));\n",pad[i].m_blokNummer);
            }
            if (pad[i].IsWissel())
            {
                printf("    expect.push_back(BaanZoekResultaat(w(%d.%02d),%d));\n",
                    mBaanInfo->IOBits[pad[i].m_wisselNummer].get()->hardwareAdres,
                    mBaanInfo->IOBits[pad[i].m_wisselNummer].get()->hardwareBit,
                    pad[i].m_stand);
            }
        }
        printf("    checkResult(expect,pad);\n");
    };
    
    void checkResult(std::vector<BaanZoekResultaat> &expect,std::vector<BaanZoekResultaat> &actual)
    {
        EXPECT_EQ(expect.size(),actual.size());
        int aantal = std::min(expect.size(),actual.size());
        for(int i=0;i<aantal;++i)
        {
            EXPECT_EQ(expect[i],actual[i])<< "Index " << i << " Blok " <<
             expect[i].m_blokNummer  << ((expect[i].m_blokNummer==actual[i].m_blokNummer)?"==":"!=") << actual[i].m_blokNummer << " wissel " << 
             expect[i].m_wisselNummer  << ((expect[i].m_wisselNummer==actual[i].m_wisselNummer)?"==":"!=") << actual[i].m_wisselNummer << " stand " <<
             expect[i].m_stand  << ((expect[i].m_stand==actual[i].m_stand)?"==":"!=") << actual[i].m_stand;
        }
        if (::testing::Test::HasFailure())
        {
            codeZoekPad(actual);
        }
    };
    int w(float num )
    {
        return objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, num);
    };
    BaanInfo_t *mBaanInfo;
protected:
  
};

TEST_F(BaanZoekTest, VooruitSimpel)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(84,RICHTING_VOORUIT,80);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(84));
    expect.push_back(BaanZoekResultaat(67));
    expect.push_back(BaanZoekResultaat(80));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, AchteruitSimpel)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(68,RICHTING_ACHTERUIT,87);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(68));
    expect.push_back(BaanZoekResultaat(83));
    expect.push_back(BaanZoekResultaat(87));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, AchteruitDoorKruising)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(87,RICHTING_ACHTERUIT,110);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(87));
    expect.push_back(BaanZoekResultaat(86));
    expect.push_back(BaanZoekResultaat(w(262.02),13));
    expect.push_back(BaanZoekResultaat(110));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, Achteruit1StandaardWissel)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(46,RICHTING_ACHTERUIT,1);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(46));
    expect.push_back(BaanZoekResultaat(44));
    expect.push_back(BaanZoekResultaat(w(256.10),13));
    expect.push_back(BaanZoekResultaat(1));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, Achteruit1StandaardWisselRecht)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(46,RICHTING_ACHTERUIT,2);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(46));
    expect.push_back(BaanZoekResultaat(44));
    expect.push_back(BaanZoekResultaat(w(256.10),12));
    expect.push_back(BaanZoekResultaat(2));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, VoorUit2StandaardWissel)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(113,RICHTING_VOORUIT,84);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(113));
    expect.push_back(BaanZoekResultaat(w(259.14),12));
    expect.push_back(BaanZoekResultaat(111));
    expect.push_back(BaanZoekResultaat(109));
    expect.push_back(BaanZoekResultaat(85));
    expect.push_back(BaanZoekResultaat(w(262.07),13));
    expect.push_back(BaanZoekResultaat(84));
    checkResult(expect,pad);
}


TEST_F(BaanZoekTest, VoorUit3StandaardWissel)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(113,RICHTING_VOORUIT,13);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(113));
    expect.push_back(BaanZoekResultaat(w(259.14),12));
    expect.push_back(BaanZoekResultaat(111));
    expect.push_back(BaanZoekResultaat(109));
    expect.push_back(BaanZoekResultaat(85));
    expect.push_back(BaanZoekResultaat(w(262.07),13));
    expect.push_back(BaanZoekResultaat(84));
    expect.push_back(BaanZoekResultaat(67));
    expect.push_back(BaanZoekResultaat(80));
    expect.push_back(BaanZoekResultaat(54));
    expect.push_back(BaanZoekResultaat(7));
    expect.push_back(BaanZoekResultaat(w(256.12),13));
    expect.push_back(BaanZoekResultaat(13));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, AchteruitBerlijnEindhoven)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(81,RICHTING_ACHTERUIT,1);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(81));
    expect.push_back(BaanZoekResultaat(95));
    expect.push_back(BaanZoekResultaat(w(262.10),12));
    expect.push_back(BaanZoekResultaat(94));
    expect.push_back(BaanZoekResultaat(w(262.06),13));
    expect.push_back(BaanZoekResultaat(48));
    expect.push_back(BaanZoekResultaat(w(262.14),13));
    expect.push_back(BaanZoekResultaat(46));
    expect.push_back(BaanZoekResultaat(44));
    expect.push_back(BaanZoekResultaat(w(256.10),13));
    expect.push_back(BaanZoekResultaat(1));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, AchteruitBerlijnEindhovenMiddenSpoor)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(70,RICHTING_ACHTERUIT,2);
    //printZoekPad(pad);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(70));
    expect.push_back(BaanZoekResultaat(90));
    expect.push_back(BaanZoekResultaat(92));
    expect.push_back(BaanZoekResultaat(w(262.00),14));
    expect.push_back(BaanZoekResultaat(w(262.04),12));
    expect.push_back(BaanZoekResultaat(93));
    expect.push_back(BaanZoekResultaat(47));
    expect.push_back(BaanZoekResultaat(w(262.05),13));
    expect.push_back(BaanZoekResultaat(w(262.06),12));
    expect.push_back(BaanZoekResultaat(48));
    expect.push_back(BaanZoekResultaat(w(262.14),13));
    expect.push_back(BaanZoekResultaat(46));
    expect.push_back(BaanZoekResultaat(44));
    expect.push_back(BaanZoekResultaat(w(256.10),12));
    expect.push_back(BaanZoekResultaat(2));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, VooruitBerlijnZijSpoorMetJuisteRichting)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(74,RICHTING_VOORUIT,76);
    //printZoekPad(pad);
    //codeZoekPad(pad);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(74));
    expect.push_back(BaanZoekResultaat(61));
    expect.push_back(BaanZoekResultaat(w(260.07),12));
    expect.push_back(BaanZoekResultaat(62));
    expect.push_back(BaanZoekResultaat(w(260.08),13));
    expect.push_back(BaanZoekResultaat(59));
    expect.push_back(BaanZoekResultaat(w(260.06),13));
    expect.push_back(BaanZoekResultaat(56));
    expect.push_back(BaanZoekResultaat(w(256.08),13));
    expect.push_back(BaanZoekResultaat(10));
    expect.push_back(BaanZoekResultaat(w(256.09),13));
    expect.push_back(BaanZoekResultaat(8));
    expect.push_back(BaanZoekResultaat(w(256.12),12));
    expect.push_back(BaanZoekResultaat(13));
    expect.push_back(BaanZoekResultaat(37));
    expect.push_back(BaanZoekResultaat(35));
    expect.push_back(BaanZoekResultaat(w(257.08),12));
    expect.push_back(BaanZoekResultaat(21));
    expect.push_back(BaanZoekResultaat(w(257.10),12));
    expect.push_back(BaanZoekResultaat(5));
    expect.push_back(BaanZoekResultaat(6));
    expect.push_back(BaanZoekResultaat(w(256.11),12));
    expect.push_back(BaanZoekResultaat(43));
    expect.push_back(BaanZoekResultaat(45));
    expect.push_back(BaanZoekResultaat(w(262.15),13));
    expect.push_back(BaanZoekResultaat(w(262.14),12));
    expect.push_back(BaanZoekResultaat(48));
    expect.push_back(BaanZoekResultaat(w(262.06),13));
    expect.push_back(BaanZoekResultaat(94));
    expect.push_back(BaanZoekResultaat(w(262.10),13));
    expect.push_back(BaanZoekResultaat(w(262.11),13));
    expect.push_back(BaanZoekResultaat(96));
    expect.push_back(BaanZoekResultaat(98));
    expect.push_back(BaanZoekResultaat(w(262.12),12));
    expect.push_back(BaanZoekResultaat(w(262.13),13));
    expect.push_back(BaanZoekResultaat(78));
    expect.push_back(BaanZoekResultaat(76));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, AchteruitBerlijnZijSpoor)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(74,RICHTING_ACHTERUIT,76);
    //codeZoekPad(pad);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(74));
    expect.push_back(BaanZoekResultaat(97));
    expect.push_back(BaanZoekResultaat(96));
    expect.push_back(BaanZoekResultaat(w(262.11),12));
    expect.push_back(BaanZoekResultaat(w(262.10),13));
    expect.push_back(BaanZoekResultaat(94));
    expect.push_back(BaanZoekResultaat(w(262.06),13));
    expect.push_back(BaanZoekResultaat(48));
    expect.push_back(BaanZoekResultaat(w(262.14),13));
    expect.push_back(BaanZoekResultaat(46));
    expect.push_back(BaanZoekResultaat(44));
    expect.push_back(BaanZoekResultaat(w(256.10),12));
    expect.push_back(BaanZoekResultaat(2));
    expect.push_back(BaanZoekResultaat(23));
    expect.push_back(BaanZoekResultaat(w(257.11),12));
    expect.push_back(BaanZoekResultaat(20));
    expect.push_back(BaanZoekResultaat(34));
    expect.push_back(BaanZoekResultaat(w(258.09),13));
    expect.push_back(BaanZoekResultaat(38));
    expect.push_back(BaanZoekResultaat(14));
    expect.push_back(BaanZoekResultaat(8));
    expect.push_back(BaanZoekResultaat(w(256.09),12));
    expect.push_back(BaanZoekResultaat(10));
    expect.push_back(BaanZoekResultaat(w(256.08),13));
    expect.push_back(BaanZoekResultaat(56));
    expect.push_back(BaanZoekResultaat(w(260.06),13));
    expect.push_back(BaanZoekResultaat(59));
    expect.push_back(BaanZoekResultaat(w(260.08),13));
    expect.push_back(BaanZoekResultaat(62));
    expect.push_back(BaanZoekResultaat(w(260.07),13));
    expect.push_back(BaanZoekResultaat(64));
    expect.push_back(BaanZoekResultaat(w(260.12),12));
    expect.push_back(BaanZoekResultaat(w(260.11),13));
    expect.push_back(BaanZoekResultaat(65));
    expect.push_back(BaanZoekResultaat(76));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, NietMogelijkOplossing)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(72,RICHTING_VOORUIT,104);
    std::vector<BaanZoekResultaat> expect;
    checkResult(expect,pad);
}


TEST_F(BaanZoekTest, WelMogelijkOplossing)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(87,RICHTING_ACHTERUIT,103);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(87));
    expect.push_back(BaanZoekResultaat(86));
    expect.push_back(BaanZoekResultaat(w(262.02),13));
    expect.push_back(BaanZoekResultaat(110));
    expect.push_back(BaanZoekResultaat(112));
    expect.push_back(BaanZoekResultaat(w(259.08),13));
    expect.push_back(BaanZoekResultaat(114));
    expect.push_back(BaanZoekResultaat(116));
    expect.push_back(BaanZoekResultaat(w(257.05),12));
    expect.push_back(BaanZoekResultaat(115));
    expect.push_back(BaanZoekResultaat(w(257.12),13));
    expect.push_back(BaanZoekResultaat(w(256.00),13));
    expect.push_back(BaanZoekResultaat(9));
    expect.push_back(BaanZoekResultaat(10));
    expect.push_back(BaanZoekResultaat(w(256.08),13));
    expect.push_back(BaanZoekResultaat(55));
    expect.push_back(BaanZoekResultaat(w(260.09),13));
    expect.push_back(BaanZoekResultaat(50));
    expect.push_back(BaanZoekResultaat(70));
    expect.push_back(BaanZoekResultaat(90));
    expect.push_back(BaanZoekResultaat(92));
    expect.push_back(BaanZoekResultaat(w(262.00),13));
    expect.push_back(BaanZoekResultaat(102));
    expect.push_back(BaanZoekResultaat(w(262.08),12));
    expect.push_back(BaanZoekResultaat(103));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, EindpuntBezet)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    mBaanInfo->Blok[13].State = BLOK_VOORUIT;
    std::vector<BaanZoekResultaat> pad = zoek.pad(84,RICHTING_VOORUIT,13);
    std::vector<BaanZoekResultaat> expect;
    checkResult(expect,pad);
    mBaanInfo->Blok[13].State = 0; // restore
}

TEST_F(BaanZoekTest, TussenPuntBezet)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    mBaanInfo->Blok[13].State = BLOK_VOORUIT;
    std::vector<BaanZoekResultaat> pad = zoek.pad(84,RICHTING_VOORUIT,37);
    std::vector<BaanZoekResultaat> expect;
    checkResult(expect,pad);
    mBaanInfo->Blok[13].State = 0; // restore
}

TEST_F(BaanZoekTest, EindhovenSpoor2Bezet)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    mBaanInfo->Blok[23].State = BLOK_VOORUIT;
    std::vector<BaanZoekResultaat> pad = zoek.pad(46,RICHTING_ACHTERUIT,38);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(46));
    expect.push_back(BaanZoekResultaat(44));
    expect.push_back(BaanZoekResultaat(w(256.10),13));
    expect.push_back(BaanZoekResultaat(1));
    expect.push_back(BaanZoekResultaat(22));
    expect.push_back(BaanZoekResultaat(w(257.11),13));
    expect.push_back(BaanZoekResultaat(20));
    expect.push_back(BaanZoekResultaat(34));
    expect.push_back(BaanZoekResultaat(w(258.09),13));
    expect.push_back(BaanZoekResultaat(38));
    checkResult(expect,pad);
    mBaanInfo->Blok[23].State = 0; // restore
}


TEST_F(BaanZoekTest, EindhovenPenaltyVoorEindResultaat)
{
    // zonder de penalty op het eind resultaat zou de afkorting via kruising 256.08 gaan
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(58,RICHTING_VOORUIT,113);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(58));
    expect.push_back(BaanZoekResultaat(57));
    expect.push_back(BaanZoekResultaat(w(260.06),13));
    expect.push_back(BaanZoekResultaat(56));
    expect.push_back(BaanZoekResultaat(11));
    expect.push_back(BaanZoekResultaat(w(257.12),13));
    expect.push_back(BaanZoekResultaat(115));
    expect.push_back(BaanZoekResultaat(113));
    checkResult(expect,pad);
}


TEST_F(BaanZoekTest, ZelfdeBlok)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(32,RICHTING_VOORUIT,32);
    EXPECT_EQ(0,pad.size());
}

TEST_F(BaanZoekTest, MiddenBerlijnNaarRandBerlijn)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(70,RICHTING_ACHTERUIT,73);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(70));
    expect.push_back(BaanZoekResultaat(90));
    expect.push_back(BaanZoekResultaat(92));
    expect.push_back(BaanZoekResultaat(w(262.00),12));
    expect.push_back(BaanZoekResultaat(w(262.03),12));
    expect.push_back(BaanZoekResultaat(88));
    expect.push_back(BaanZoekResultaat(86));
    expect.push_back(BaanZoekResultaat(w(262.02),12));
    expect.push_back(BaanZoekResultaat(110));
    expect.push_back(BaanZoekResultaat(112));
    expect.push_back(BaanZoekResultaat(w(259.08),13));
    expect.push_back(BaanZoekResultaat(114));
    expect.push_back(BaanZoekResultaat(116));
    expect.push_back(BaanZoekResultaat(w(257.05),13));
    expect.push_back(BaanZoekResultaat(12));
    expect.push_back(BaanZoekResultaat(59));
    expect.push_back(BaanZoekResultaat(w(260.08),13));
    expect.push_back(BaanZoekResultaat(60));
    expect.push_back(BaanZoekResultaat(73));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, NietDoorWaalre)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    mBaanInfo->Blok[109].State = BLOK_VOORUIT;
    mBaanInfo->Blok[112].State = BLOK_VOORUIT;
    mBaanInfo->Blok[13].State = BLOK_VOORUIT;
    mBaanInfo->Blok[21].State = BLOK_VOORUIT;
    mBaanInfo->RegelArray[0].ELoc = 1;
    std::vector<BaanZoekResultaat> pad = zoek.pad(58,RICHTING_VOORUIT,50,0);
    mBaanInfo->RegelArray[0].ELoc = 0;
    mBaanInfo->Blok[109].State = 0;
    mBaanInfo->Blok[112].State = 0;
    mBaanInfo->Blok[13].State = 0;
    mBaanInfo->Blok[21].State = 0;
    std::vector<BaanZoekResultaat> expect;
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, EindhovenZelfdeSpoor)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    std::vector<BaanZoekResultaat> pad = zoek.pad(4,RICHTING_VOORUIT,3,0);
    std::vector<BaanZoekResultaat> expect;
    expect.push_back(BaanZoekResultaat(4));
    expect.push_back(BaanZoekResultaat(w(256.11),13));
    expect.push_back(BaanZoekResultaat(43));
    expect.push_back(BaanZoekResultaat(45));
    expect.push_back(BaanZoekResultaat(w(262.15),12));
    expect.push_back(BaanZoekResultaat(w(262.05),13));
    expect.push_back(BaanZoekResultaat(47));
    expect.push_back(BaanZoekResultaat(82));
    expect.push_back(BaanZoekResultaat(72));
    expect.push_back(BaanZoekResultaat(58));
    expect.push_back(BaanZoekResultaat(57));
    expect.push_back(BaanZoekResultaat(w(260.06),12));
    expect.push_back(BaanZoekResultaat(56));
    expect.push_back(BaanZoekResultaat(w(256.08),13));
    expect.push_back(BaanZoekResultaat(10));
    expect.push_back(BaanZoekResultaat(w(256.09),13));
    expect.push_back(BaanZoekResultaat(8));
    expect.push_back(BaanZoekResultaat(w(256.12),12));
    expect.push_back(BaanZoekResultaat(13));
    expect.push_back(BaanZoekResultaat(37));
    expect.push_back(BaanZoekResultaat(35));
    expect.push_back(BaanZoekResultaat(w(257.08),12));
    expect.push_back(BaanZoekResultaat(21));
    expect.push_back(BaanZoekResultaat(w(257.10),13));
    expect.push_back(BaanZoekResultaat(3));
    checkResult(expect,pad);
}

TEST_F(BaanZoekTest, BlokListLengte)
{
    BaanInfo info(mBaanInfo);
    BaanZoek zoek(info,objects->wissels);
    EXPECT_EQ(-22 + 37      , zoek.BlokListLengte(0, w(256.00),2,0));
    EXPECT_EQ(-22 + 22 + 160, zoek.BlokListLengte(0, w(256.00),1,0));
    EXPECT_EQ( 37 + 46      , zoek.BlokListLengte(2, w(256.00),0,0));
    EXPECT_EQ( 22 + 46      , zoek.BlokListLengte(1, w(256.00),0,0));
    EXPECT_EQ(40      , zoek.BlokListLengte(0, w(256.08),1,0));
    EXPECT_EQ(46 + 22 , zoek.BlokListLengte(0, w(256.08),2,0));
    EXPECT_EQ(38      , zoek.BlokListLengte(3, w(256.08),1,0));
    EXPECT_EQ(60 + 22 , zoek.BlokListLengte(3, w(256.08),2,0));
    EXPECT_EQ(-46 + 40 + 58, zoek.BlokListLengte(1, w(256.08),0,0));
    EXPECT_EQ(-46 + 46 + 58, zoek.BlokListLengte(2, w(256.08),0,0));
    EXPECT_EQ(-46 + 38 + 25, zoek.BlokListLengte(1, w(256.08),3,0));
    EXPECT_EQ(-46 + 60 + 25, zoek.BlokListLengte(2, w(256.08),3,0));
}

TEST_F(BaanZoekTest, ProgramInit)
{
    BaanInfo info(mBaanInfo);

    CProgramma prog(objects->message, objects->wissels, objects->baanMessage, objects->mainWindowDrawing, objects->td, objects->errorPrint, mBaanInfo);
    BlokPointer_t *blok = info.GetBlokPointer(81);
    mBaanInfo->RegelArray[0].pKopBlok = blok; // init de regelaar met een blok
    mBaanInfo->RegelArray[0].Richting = 1; // achteruit
    prog.executeProgram (INIT, 0); // regelaar 0 initalizatie
    int retSnel = prog.zetSnelheid(10);
    EXPECT_EQ(0, retSnel);
    retSnel = prog.zetSnelheid(0);
    EXPECT_EQ(0, retSnel);  // omdat de maxsnelheid van een ongeinitializeerde regelaar 0 is
    EXPECT_TRUE(prog.zoekEnBeleg(1));
    // test of de regelaar het destination blok belegd
    EXPECT_EQ(1,mBaanInfo->Blok[1].blokBelegt);
    prog.verwijderAanvragen();
    EXPECT_EQ(0,mBaanInfo->Blok[1].blokBelegt);
    EXPECT_EQ(1 , prog.GetTreinBlok("Eindhoven","Spoor 1"));
    EXPECT_EQ(22 , prog.GetStopBlok("Eindhoven","Spoor 1"));
    EXPECT_TRUE(prog.IsBlokAanwezig("Eindhoven","Spoor 2", 2));
    EXPECT_FALSE(prog.IsBlokAanwezig("Eindhoven","Spoor 2", 15));

    CProgramma prog2(objects->message, objects->wissels, objects->baanMessage, objects->mainWindowDrawing, objects->td, objects->errorPrint, mBaanInfo);
    blok = info.GetBlokPointer(81);
    mBaanInfo->RegelArray[1].pKopBlok = blok; // init de regelaar met een blok
    mBaanInfo->RegelArray[1].Richting = 1; // achteruit
    prog2.executeProgram (INIT, 1); 
    EXPECT_EQ(0, retSnel);  // omdat de maxsnelheid van een ongeinitializeerde regelaar 0 is
    EXPECT_TRUE(prog2.zoekEnBeleg(38));
}

TEST_F(BaanZoekTest, NextBlokVooruit)
{
    BaanInfo info(mBaanInfo);
    BlokPointer_t *blok = info.GetBlokPointer(84);
    BaanZoekResult res=info.NextBlok(blok,RICHTING_VOORUIT);
    ASSERT_EQ(67.0f, res.nummer);
    ASSERT_TRUE(res.isBlok());
    ASSERT_EQ(info.GetBlokPointer(67), res.nextBlok);
    ASSERT_EQ(1, res.aantalMogelijkheden());
}

TEST_F(BaanZoekTest, NextBlokAchteruit)
{
    BaanInfo info(mBaanInfo);
    BlokPointer_t *blok = info.GetBlokPointer(67);
    BaanZoekResult res=info.NextBlok(blok,RICHTING_ACHTERUIT);
    ASSERT_EQ(84.0f, res.nummer);
    ASSERT_TRUE(res.isBlok());
    ASSERT_EQ(info.GetBlokPointer(84), res.nextBlok);
    ASSERT_EQ(1, res.aantalMogelijkheden());
}

TEST_F(BaanZoekTest, NextBlokEindBlok)
{
    BaanInfo info(mBaanInfo);
    BlokPointer_t *blok = info.GetBlokPointer(104);
    BaanZoekResult res=info.NextBlok(blok,RICHTING_ACHTERUIT);
    ASSERT_EQ(0.0f, res.nummer);
    ASSERT_FALSE(res.isBlok());
    ASSERT_EQ(NULL, res.nextBlok);
    ASSERT_EQ(0, res.aantalMogelijkheden());
}

TEST_F(BaanZoekTest, InitStandaarWisselVooruit)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.11);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].aansluitingen.size());
    ASSERT_EQ(1,wissel->routeKnoopPunt[0].aansluitingen[0]);
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].aansluitingen[1]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].aansluitingen.size());
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].aansluitingen[0]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[2].aansluitingen.size());
    ASSERT_EQ(0,wissel->routeKnoopPunt[2].aansluitingen[0]);
    ASSERT_EQ(0,wissel->routeKnoopPunt[3].aansluitingen.size());
    ASSERT_EQ(3,wissel->routeKnoopPunt[0].blokList.size());
    ASSERT_EQ(65,wissel->routeKnoopPunt[0].blokList[0]);
    ASSERT_EQ(76,wissel->routeKnoopPunt[0].blokList[1]);
    ASSERT_EQ(78,wissel->routeKnoopPunt[0].blokList[2]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.13),
        wissel->routeKnoopPunt[0].wisselNummer);

    ASSERT_EQ(1,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(66,wissel->routeKnoopPunt[1].blokList[0]);
    ASSERT_EQ(-1,wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(-1,wissel->routeKnoopPunt[1].knoopPunt);

    ASSERT_EQ(0,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.12),
        wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(1,wissel->routeKnoopPunt[2].knoopPunt);
}

TEST_F(BaanZoekTest, InitStandaarWisselVooruitTeLang)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.15);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.05),
        wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].knoopPunt);

    ASSERT_EQ(0,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.14),
        wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(1,wissel->routeKnoopPunt[2].knoopPunt);
}

TEST_F(BaanZoekTest, InitStandaarWisselWisselVooruit)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.0);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(15,wissel->routeKnoopPunt[1].blokList[0]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 258.08),
        wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].knoopPunt);
    
    ASSERT_EQ(0,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 257.12),
        wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(3,wissel->routeKnoopPunt[2].knoopPunt);
}

TEST_F(BaanZoekTest, InitStandaarWisselAchteruit)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.10);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].blokList.size());
    ASSERT_EQ(44,wissel->routeKnoopPunt[0].blokList[0]);
    ASSERT_EQ(46,wissel->routeKnoopPunt[0].blokList[1]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.14),
        wissel->routeKnoopPunt[0].wisselNummer);
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].knoopPunt);
}

TEST_F(BaanZoekTest, InitStandaarWisselAchteruit1Blok2Wissels)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.14);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(1,wissel->routeKnoopPunt[0].blokList.size());
    ASSERT_EQ(48,wissel->routeKnoopPunt[0].blokList[0]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.06),
        wissel->routeKnoopPunt[0].wisselNummer);
    ASSERT_EQ(0,wissel->routeKnoopPunt[0].knoopPunt);
}

TEST_F(BaanZoekTest, InitStandaarWisselAchteruitEindblokTerug)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.13);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(100,wissel->routeKnoopPunt[1].blokList[0]);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[1].knoopPunt);
}

TEST_F(BaanZoekTest, InitDriewegWisselAchteruit)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.00);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(3,wissel->routeKnoopPunt[0].aansluitingen.size());
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].aansluitingen[0]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[0].aansluitingen[1]);
    ASSERT_EQ(3,wissel->routeKnoopPunt[0].aansluitingen[2]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].aansluitingen.size());
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].aansluitingen[0]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[2].aansluitingen.size());
    ASSERT_EQ(0,wissel->routeKnoopPunt[2].aansluitingen[0]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[3].aansluitingen.size());
    ASSERT_EQ(0,wissel->routeKnoopPunt[3].aansluitingen[0]);
    ASSERT_EQ(4,wissel->routeKnoopPunt[0].blokList.size());
    ASSERT_EQ(92,wissel->routeKnoopPunt[0].blokList[0]);
    ASSERT_EQ(90,wissel->routeKnoopPunt[0].blokList[1]);
    ASSERT_EQ(70,wissel->routeKnoopPunt[0].blokList[2]);
    ASSERT_EQ(50,wissel->routeKnoopPunt[0].blokList[3]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.09),
        wissel->routeKnoopPunt[0].wisselNummer);
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].knoopPunt);
}

TEST_F(BaanZoekTest, InitDriewegWisselEindblokTerug)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.08);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(103,wissel->routeKnoopPunt[1].blokList[0]);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[1].knoopPunt);
    ASSERT_EQ(1,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(104,wissel->routeKnoopPunt[2].blokList[0]);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[2].knoopPunt);
    ASSERT_EQ(1,wissel->routeKnoopPunt[3].blokList.size());
    ASSERT_EQ(105,wissel->routeKnoopPunt[3].blokList[0]);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[3].wisselNummer);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[3].knoopPunt);
}


TEST_F(BaanZoekTest, InitStandaardWisselAchteruitDrieweg4)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.04);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.00),
        wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(3,wissel->routeKnoopPunt[1].knoopPunt);

    ASSERT_EQ(3,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.09),
        wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(3,wissel->routeKnoopPunt[2].knoopPunt);
}

TEST_F(BaanZoekTest, InitStandaardWisselAchteruitDrieweg2)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.03);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.00),
        wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].knoopPunt);

    ASSERT_EQ(3,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.09),
        wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(1,wissel->routeKnoopPunt[2].knoopPunt);
}

TEST_F(BaanZoekTest, InitEngelseWissel)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.08);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].aansluitingen.size());
    ASSERT_EQ(2,wissel->routeKnoopPunt[0].aansluitingen[0]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[0].aansluitingen[1]);
    ASSERT_EQ(2,wissel->routeKnoopPunt[1].aansluitingen.size());
    ASSERT_EQ(3,wissel->routeKnoopPunt[1].aansluitingen[0]);
    ASSERT_EQ(0,wissel->routeKnoopPunt[1].aansluitingen[1]);
    ASSERT_EQ(2,wissel->routeKnoopPunt[2].aansluitingen.size());
    ASSERT_EQ(0,wissel->routeKnoopPunt[2].aansluitingen[0]);
    ASSERT_EQ(3,wissel->routeKnoopPunt[2].aansluitingen[1]);
    ASSERT_EQ(2,wissel->routeKnoopPunt[3].aansluitingen.size());
    ASSERT_EQ(1,wissel->routeKnoopPunt[3].aansluitingen[0]);
    ASSERT_EQ(2,wissel->routeKnoopPunt[3].aansluitingen[1]);
    ASSERT_EQ(1,wissel->routeKnoopPunt[1].blokList.size());
    ASSERT_EQ(10,wissel->routeKnoopPunt[1].blokList[0]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.09),
        wissel->routeKnoopPunt[1].wisselNummer);
    ASSERT_EQ(3,wissel->routeKnoopPunt[1].knoopPunt);

    ASSERT_EQ(2,wissel->routeKnoopPunt[2].blokList.size());
    ASSERT_EQ(10,wissel->routeKnoopPunt[2].blokList[0]);
    ASSERT_EQ(9,wissel->routeKnoopPunt[2].blokList[1]);
    ASSERT_EQ(
        objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.00),
        wissel->routeKnoopPunt[2].wisselNummer);
    ASSERT_EQ(0,wissel->routeKnoopPunt[2].knoopPunt);
}

TEST_F(BaanZoekTest, InitEngelseWisselEindblokTerug)
{
    BaanInfo info(mBaanInfo);
    int wisselNummer = objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.11);
    IOBits_t * wissel = mBaanInfo->IOBits[wisselNummer].get();
    ASSERT_EQ(1,wissel->routeKnoopPunt[0].blokList.size());
    ASSERT_EQ(99,wissel->routeKnoopPunt[0].blokList[0]);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[0].wisselNummer);
    ASSERT_EQ(-1, wissel->routeKnoopPunt[0].knoopPunt);
}


TEST_F(BaanZoekTest, TestInitialization)
{
    BaanInfo info(mBaanInfo);
    int countBloks[MAX_NOBLOKS];
    for (int i=0;i<MAX_NOBLOKS;i++)
    {
        countBloks[i] = 0;
    }
    for (int i=0;i<mBaanInfo->AantalSpoelen;i++)
    {
        IOBits_t * pWissel = mBaanInfo->IOBits[i].get();
        switch (pWissel->Type)
        {
        case KRUISING:
        case ENGELSE_WISSEL:
            // deze wissels hebben het eerst blok 2 maal
            countBloks[pWissel->GetWisselBlok()->BlokIONummer] -= 2;
        case STANDAARD_WISSEL:
        case DRIEWEG_WISSEL:
          for(int y=0;y<4;y++)
          {
              for(unsigned int x=0;x<pWissel->routeKnoopPunt[y].blokList.size();x++)
              {
                  countBloks[pWissel->routeKnoopPunt[y].blokList[x]] += 1;
              }
          }
          break;
        }
    }
    for (int i=0;i<MAX_NOBLOKS;i++)
    {
        if (mBaanInfo->BlokPointer[i].BlokIONummer > 0)
        {
            switch(countBloks[i])
            {
            case 0:
                break; // voor een los blok
            case 1:
                EXPECT_TRUE((mBaanInfo->BlokPointer[i].pVorigBlok == &mBaanInfo->EindBlokPointer) ||
                (mBaanInfo->BlokPointer[i].pVolgendBlok == &mBaanInfo->EindBlokPointer))
                << "Geen eindblok voor blok " << i;
                break;
            case 2:
                break;
            default:
            EXPECT_EQ(2, countBloks[i]) << "Voor Blok " << i;
            }
        }
    }
}


TEST_F(BaanZoekTest, VindEersteWissel)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(5,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.11),res.wisselNummer);
   ASSERT_EQ(1 ,res.knoopPuntNummer);

}

TEST_F(BaanZoekTest, VindEersteWisselStanaard1Achteruit)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(5,1);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 257.10),res.wisselNummer);
   ASSERT_EQ(1 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteWisselStandaard2Vooruit)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(4,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 256.11),res.wisselNummer);
   ASSERT_EQ(2 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteWisselStandaard2Achteruit)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(4,1);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 257.10),res.wisselNummer);
   ASSERT_EQ(2 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteEngelseWissel)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(74,1);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.11),res.wisselNummer);
   ASSERT_EQ(2 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteEngelseWisselVoor)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(74,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.07),res.wisselNummer);
   ASSERT_EQ(0 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteEngelseWisselaansluiting1)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(93,1);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.05),res.wisselNummer);
   ASSERT_EQ(1 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteEngelseWisselaansluiting3)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(64,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.07),res.wisselNummer);
   ASSERT_EQ(3 ,res.knoopPuntNummer);
}


TEST_F(BaanZoekTest, VindEersteNiets)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(105,1);
   
   ASSERT_EQ(-1
       ,res.wisselNummer);
   ASSERT_EQ(-1 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteDriewegAansluiting1)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(69,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 260.09),res.wisselNummer);
   ASSERT_EQ(1 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteDriewegAansluiting2)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(102,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.00),res.wisselNummer);
   ASSERT_EQ(2 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteStandaard2maalVooruit)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(48,0);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.06),res.wisselNummer);
   ASSERT_EQ(0 ,res.knoopPuntNummer);
}

TEST_F(BaanZoekTest, VindEersteStandaard2maalAchteruit)
{
   BaanInfo info(mBaanInfo);
   EersteWisselResult res = objects->wissels.VindEersteWissel(48,1);
   
   ASSERT_EQ(
       objects->wissels.ZoekWisselNummer (mBaanInfo->IOBits, 262.14),res.wisselNummer);
   ASSERT_EQ(0 ,res.knoopPuntNummer);
}
