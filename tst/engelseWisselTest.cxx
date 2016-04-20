#include "gtest/gtest.h"
#include "baanLibSetup.h"

#include "chrono"
#include "future"

using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::InSequence;

class engelseWisselTest : public ::testing::Test {
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

TEST_F(engelseWisselTest, Constructie)
{
    InitBlok(1);
    InitBlok(2);
    InitBlok(3);
    InitBlok(4);
    InitBlok(5);
    InitWissel(0,ENGELSE_WISSEL,100 , 01);
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
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    std::promise<void> done1;
    {
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done1] (const hardwareArray_t & data){
            EXPECT_EQ(100,data.adres);
            EXPECT_EQ(1+(3<<4),data.data); // get status
            done1.set_value();
            return 0;
        }));
    }

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(12));
    EXPECT_EQ(12,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(std::future_status::ready, done1.get_future().wait_for(std::chrono::seconds(1)));

    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(5, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[4].pVolgendBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(3, baanInfo->BlokPointer[4].pVolgendBlok->pVolgendBlok->BlokIONummer);

    //
    // bedien de wissel in de andere stand en check de blokken
    //
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));
    std::promise<void> done2;
    {
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done2] (const hardwareArray_t & data){
            EXPECT_EQ(100,data.adres);
            EXPECT_EQ(1,data.data); // get status
            done2.set_value();
            return 0;
        }));
    }

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(13));
    EXPECT_EQ(13,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(std::future_status::ready, done2.get_future().wait_for(std::chrono::seconds(1)));
    EXPECT_EQ(2, baanInfo->BlokPointer[1].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(3, baanInfo->BlokPointer[2].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(0, baanInfo->BlokPointer[4].pVolgendBlok->BlokIONummer);
    EXPECT_EQ(&baanInfo->Blok[2], baanInfo->BlokPointer[4].pVolgendBlok->pBlok); // wissel blok moet naar blok 2 wijzen
    EXPECT_EQ(5, baanInfo->BlokPointer[4].pVolgendBlok->pVolgendBlok->BlokIONummer);


}
