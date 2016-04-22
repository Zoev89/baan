#include "gtest/gtest.h"
#include "baanLibSetup.h"

#include "chrono"
#include "future"

using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::InSequence;

class wisselTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        baanInfo = std::move(make_unique<BaanInfo_t>(mHardwareHoog, mHardwareLaag));
        objects =  std::move(make_unique<InitObjects>(baanInfo.get())); // Program Init fails want de objects zijn niet geinitializeerd bij mBaanInfo new hier boven
        EXPECT_CALL(objects->threadSleep,SleepFor(_)).WillRepeatedly(Return());
    }

    virtual void TearDown()
    {
        baanInfo.reset();
        objects.reset();
    }

    void InitBlok(int blok)
    {
        baanInfo->BlokPointer[blok].BlokIONummer = blok;

    }

    void InitWissel(int wissel, int type, int adres, int bit)
    {
        objects->wissels.CreateNewIO(type);
        auto pWissel = baanInfo->IOBits[wissel].get();

        pWissel->hardwareAdres = adres;
        pWissel->hardwareBit = bit;
    }


    IHardwareComMock mHardwareHoog;
    IHardwareComMock mHardwareLaag;
    std::unique_ptr<InitObjects> objects;
    std::unique_ptr<BaanInfo_t> baanInfo;
};

TEST_F(wisselTest, engelseWissel)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,ENGELSE_WISSEL,100 , 01);
    //   4-> \          |
    //   1 -> 2 -> 3    |
    //         \-> 5    |
    objects->blok.InitBlok("1      B2  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("2      B3  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("3      B-1  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("4      W100.01  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("5      B-1  -1 134 0 3 2  100  100");
    EXPECT_EQ(0,objects->wissels.Init (0, "4  100.01  714  644  753  649  754  644  716  639       2  B5  140  123   26   42   -1   -1"
        , []()
        {
            return std::string();
        }));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[4].pVolgendBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(5, baanInfo->BlokPointer[4].pVolgendBlok->pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[5].pVorigBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(4, baanInfo->BlokPointer[5].pVorigBlok->pVorigBlok->BlokIONummer);

    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        EXPECT_EQ(1+(3<<4),data.data); // get status
        return 0;
    }));

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);

    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(5, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[4].pVolgendBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(3, baanInfo->BlokPointer[4].pVolgendBlok->pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[3].pVorigBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(4, baanInfo->BlokPointer[3].pVorigBlok->pVorigBlok->BlokIONummer);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        EXPECT_EQ(1,data.data); // get status
        return 0;
    }));

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(13));
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[4].pVolgendBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(5, baanInfo->BlokPointer[4].pVolgendBlok->pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[5].pVorigBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(4, baanInfo->BlokPointer[5].pVorigBlok->pVorigBlok->BlokIONummer);


    // met een rijdende trein geen bediening
    baanInfo->Blok[2].RegelaarNummer = 0;
    baanInfo->Blok[2].State = BLOK_VOORUIT;
    baanInfo->Blok[2].Snelheid = 10;
    EXPECT_EQ(IOGEWIJGERD,baanInfo->IOBits[0]->Aanvraag(12));
}


TEST_F(wisselTest, kruising)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,KRUISING,100 , 01);
    //   4-> \          |
    //   1 -> 2 -> 3    |
    //         \-> 5    |
    objects->blok.InitBlok("1      B2  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("2      B3  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("3      B-1  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("4      W100.01  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("5      B-1  -1 134 0 3 2  100  100");
    EXPECT_EQ(0,objects->wissels.Init (0, "3  100.01  714  644  753  649  754  644  716  639       2  B5  140  123   26   42   -1   -1"
        , []()
        {
            return std::string();
        }));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[4].pVolgendBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(5, baanInfo->BlokPointer[4].pVolgendBlok->pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[5].pVorigBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(4, baanInfo->BlokPointer[5].pVorigBlok->pVorigBlok->BlokIONummer);

    EXPECT_EQ(0,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Stand);

}


TEST_F(wisselTest, standaardWisselVoorWaards)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,STANDAARD_WISSEL,100 , 01);
    //   1 -> 2 -> 3    |
    //         \-> 4    |
    objects->blok.InitBlok("1      B2  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("2      B3  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("3      B-1  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("4      B-1  -1 134 0 3 2  100  100");
    EXPECT_EQ(0,objects->wissels.Init (0, "2  100.01  853  338  853  370  861  370        2  B4   22   37   -1   -1"
        , []()
        {
            return std::string();
        }));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        EXPECT_EQ(1+(3<<4),data.data); // get status
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(13));
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);

    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        EXPECT_EQ(1,data.data); // get status
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);

}


TEST_F(wisselTest, standaardWisselAchterWaards)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,STANDAARD_WISSEL,100 , 01);
    //   1 -> 2 ->  3   |
    //   4 ->/          |
    objects->blok.InitBlok("1      B2  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("2      B3  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("3      B-1  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("4      W100.01  -1 134 0 3 2  100  100");
    EXPECT_EQ(0,objects->wissels.Init (0, "2  100.01  853  338  853  370  861  370        2  B-1   22   37   -1   -1"
        , []()
        {
            return std::string();
        }));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        EXPECT_EQ(1+(3<<4),data.data); // get status
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(13));
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);

    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        EXPECT_EQ(1,data.data); // get status
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);

}


TEST_F(wisselTest, driewegWisselVoorWaards)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,DRIEWEG_WISSEL,100 , 01);
    //         /-> 3    |
    //   1 -> 2 -> 4    |
    //         \-> 5    |
    objects->blok.InitBlok("1      B2  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("2      B4  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("3      B-1  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("4      B-1  -1 134 0 3 2  100  100");
    EXPECT_EQ(0,objects->wissels.Init (0, "5  260.09  779  200  748  209  738  200  741  193       2      B3      B5  -58  -58  -58   -1   -1   -1"
        , []()
        {
            return std::string();
        }));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    bool eersteCommando = true;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).Times(2).WillRepeatedly(Invoke( [&eersteCommando] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        if(eersteCommando)
        {
            EXPECT_EQ(1+(3<<4),data.data);
        }
        else
        {
            EXPECT_EQ(2,data.data);
        }
        eersteCommando = false;
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    eersteCommando = true;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).Times(2).WillRepeatedly(Invoke( [&eersteCommando] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        if(eersteCommando)
        {
            EXPECT_EQ(1,data.data);
        }
        else
        {
            EXPECT_EQ(2+(3<<4),data.data);
        }
        eersteCommando = false;
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(14));
    EXPECT_EQ(14,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(5, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    eersteCommando = true;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).Times(2).WillRepeatedly(Invoke( [&eersteCommando] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        if(eersteCommando)
        {
            EXPECT_EQ(1,data.data);
        }
        else
        {
            EXPECT_EQ(2,data.data);
        }
        eersteCommando = false;
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(13));
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVorigBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[5].pVorigBlok->BlokIONummer);
}

TEST_F(wisselTest, driewegWisselAchterWaards)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,DRIEWEG_WISSEL,100 , 01);
    //   3 ->\          |
    //   1 -> 2 -> 4    |
    //   5 ->/          |
    objects->blok.InitBlok("1      B2  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("2      B4  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("4      B-1  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("3      W100.01  -1 134 0 3 2  100  100");
    objects->blok.InitBlok("5      w100.01  -1 134 0 3 2  100  100");
    EXPECT_EQ(0,objects->wissels.Init (0, "5  260.09  779  200  748  209  738  200  741  193       2      B-1      B-1  -58  -58  -58   -1   -1   -1"
        , []()
        {
            return std::string();
        }));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[5].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    bool eersteCommando = true;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).Times(2).WillRepeatedly(Invoke( [&eersteCommando] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        if(eersteCommando)
        {
            EXPECT_EQ(1+(3<<4),data.data);
        }
        else
        {
            EXPECT_EQ(2,data.data);
        }
        eersteCommando = false;
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[3].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[5].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    eersteCommando = true;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).Times(2).WillRepeatedly(Invoke( [&eersteCommando] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        if(eersteCommando)
        {
            EXPECT_EQ(1,data.data);
        }
        else
        {
            EXPECT_EQ(2+(3<<4),data.data);
        }
        eersteCommando = false;
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(14));
    EXPECT_EQ(14,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[5].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(5, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    eersteCommando = true;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).Times(2).WillRepeatedly(Invoke( [&eersteCommando] (const hardwareArray_t & data){
        EXPECT_EQ(100,data.adres);
        if(eersteCommando)
        {
            EXPECT_EQ(1,data.data);
        }
        else
        {
            EXPECT_EQ(2,data.data);
        }
        eersteCommando = false;
        return 0;
    }));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(13));
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(4, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[3].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(-1, baanInfo->BlokPointer[5].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(1, baanInfo->BlokPointer[2].pVorigBlok->BlokIONummer);
    EXPECT_EQ(2, baanInfo->BlokPointer[4].pVorigBlok->BlokIONummer);

}
