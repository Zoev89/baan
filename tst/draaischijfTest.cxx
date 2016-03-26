
#include "gtest/gtest.h"
#include "baanLibSetup.h"
#include "chrono"
#include "thread"

//using ::testing::_;

using ::testing::Return;
using ::testing::_;

class draaischijfTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        baanInfo = std::move(make_unique<BaanInfo_t>());
        objects =  std::move(make_unique<InitObjects>(baanInfo.get())); // Program Init fails want de objects zijn niet geinitializeerd bij mBaanInfo new hier boven
    }

    virtual void TearDown()
    {
        objects.reset();
        baanInfo.reset();
    }

    hardwareArray_t PopItemFromHardwareHoog()
    {
        hardwareArray_t data;
        data.adres= 0;
        data.data =0;
        int count = 0;
        while ((count <5) && (baanInfo->hardwareHoog.krijgItem (&data)==1))
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            count += 1;
        }
        EXPECT_LT(count, 5);
        return data;
    }

    std::unique_ptr<BaanInfo_t> baanInfo;
    std::unique_ptr<InitObjects> objects;

};

TEST_F(draaischijfTest, Constructie)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 1000 100 50 0 0",[]()
    {
        std::string ret;
        return ret;
    }));
    auto output = objects->wissels.String(0);

    EXPECT_EQ("7  300.00 1000  100   50    0    0",output);
}


TEST_F(draaischijfTest, ZetWisselStand)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 1000 100 50 0 0",[]()
    {
        std::string ret;
        return ret;
    }));
    baanInfo->IOBits[0]->hardwareReturnWaarde = 0; // return waarde van de hardware is klaar
    EXPECT_EQ(IOGEWIJGERD,baanInfo->IOBits[0]->Aanvraag(12));

    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0));

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(112));
    EXPECT_EQ(112,baanInfo->IOBits[0]->Stand);

    auto data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(63,data.data); // get status

    data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(12,data.data);
}

TEST_F(draaischijfTest, UIWisselStand)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 1000 100 50 0 0",[]()
    {
        std::string ret;
        return ret;
    }));
    baanInfo->IOBits[0]->hardwareReturnWaarde = 0; // return waarde van de hardware is klaar

    // default zit het huisje aan de onderkant
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0)).Times(4);
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE));
    // click drag bovenkant midden
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE | (1000<<16) | (100-40)  ));
    // click drag rechts midden
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE | (1040<<16) | 100  ));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE));
    EXPECT_EQ(212,baanInfo->IOBits[0]->Stand);


    auto data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(63,data.data); // get status

    data = PopItemFromHardwareHoog();
    EXPECT_EQ(303,data.adres);
    EXPECT_EQ(36,data.data);

    data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(63,data.data); // get status


    // nu zit het huisje links midden draai 180 graden
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0)).Times(4);
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE));
    // click drag links midden
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE | (960<<16) | (100)  ));
    // click drag rechts midden
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE | (1040<<16) | 100  ));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE));
    EXPECT_EQ(112,baanInfo->IOBits[0]->Stand);

    data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(63,data.data); // get status

    data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(12,data.data);

    data = PopItemFromHardwareHoog();
    EXPECT_EQ(302,data.adres);
    EXPECT_EQ(63,data.data); // get status


}

TEST_F(draaischijfTest, UIWisselStandMetTrein)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 1000 100 50 0 1",[]()
    {
        return "6 1 100 0";
    }));
    baanInfo->IOBits[0]->hardwareReturnWaarde = 0; // return waarde van de hardware is klaar
}
