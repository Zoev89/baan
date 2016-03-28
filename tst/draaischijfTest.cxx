
#include "gtest/gtest.h"
#include "baanLibSetup.h"
#include "chrono"
#include "thread"
#include "future"

using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::InSequence;

class draaischijfTest : public ::testing::Test {
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

    IHardwareComMock mHardwareHoog;
    IHardwareComMock mHardwareLaag;
    std::unique_ptr<InitObjects> objects;
    std::unique_ptr<BaanInfo_t> baanInfo;
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
    std::promise<void> done;
    {
        InSequence in;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(63,data.data); // get status
        return 0;
    }));

    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(12,data.data);
        return 0;
    }));

    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(63,data.data); // get status
        done.set_value();
        return 0;
    }));
    }


    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(112));
    EXPECT_EQ(112,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(std::future_status::ready, done.get_future().wait_for(std::chrono::seconds(1)));
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


    std::promise<void> done1;
    {
        InSequence in;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(63,data.data); // get status
        return 0;
    }));

    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(303,data.adres);
        EXPECT_EQ(36,data.data);
        return 0;
    }));

    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done1] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(63,data.data); // get status
        done1.set_value();
        return 0;
    }));
    }


    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE)); // nu wordt die bedient
    EXPECT_EQ(212,baanInfo->IOBits[0]->Stand);

    EXPECT_EQ(std::future_status::ready, done1.get_future().wait_for(std::chrono::seconds(1)));


    // nu zit het huisje links midden draai 180 graden
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0)).Times(4);
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE));
    // click drag links midden
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE | (960<<16) | (100)  ));
    // click drag rechts midden
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE | (1040<<16) | 100  ));

    std::promise<void> done2;
    {
        InSequence in;
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(63,data.data); // get status
        return 0;
    }));

    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(12,data.data);
        return 0;
    }));

    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done2] (const hardwareArray_t & data){
        EXPECT_EQ(302,data.adres);
        EXPECT_EQ(63,data.data); // get status
        done2.set_value();
        return 0;
    }));
    }


    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(IOAANVRAAG_TOGGLE));// nu wordt die bedient
    EXPECT_EQ(112,baanInfo->IOBits[0]->Stand);
    EXPECT_EQ(std::future_status::ready, done2.get_future().wait_for(std::chrono::seconds(1)));

}

TEST_F(draaischijfTest, UIWisselStandMetTrein)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0)).Times(3); // 3 testen
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 1000 100 50 0 1",[]()
    {
        return "0 1 100 0"; // default 0 adres
    }));
    // een trein met snelheid
    baanInfo->IOBits[0]->hardwareReturnWaarde = 0; // return waarde van de hardware is klaar
    baanInfo->Blok[300].State = BLOK_VOORUIT;
    baanInfo->Blok[300].Snelheid = 10;
    EXPECT_EQ(IOGEWIJGERD,baanInfo->IOBits[0]->Aanvraag(112));
    baanInfo->Blok[300].State = BLOK_VOORUIT;
    baanInfo->Blok[300].Snelheid = 0;
    baanInfo->Blok[100].State = BLOK_VOORUIT; // blok op aansluiting 100 is niet vrij dus de trein staat er overheen
    EXPECT_EQ(IOGEWIJGERD,baanInfo->IOBits[0]->Aanvraag(112));
    baanInfo->Blok[100].State = BLOK_VRIJ; // blok op aansluiting 100 is niet vrij dus de trein staat er overheen
    EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillRepeatedly(Return(0));
    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(112));

}


TEST_F(draaischijfTest, RelaisControle)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_CALL(objects->baanMessage, Post(WM_WISSEL_DISPLAY, 0, 0, 0)).WillRepeatedly(Return(0)); // 3 testen
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 1000 100 50 0 2",[]()
    {
        static int aantal=0;
        aantal += 1;
        if (aantal==1) return "0 1 100 0"; // default 0 adres
        else return "3 0 -1 0"; // adres 3 geen gnd first
    }));
    // een trein met snelheid
    baanInfo->IOBits[0]->hardwareReturnWaarde = 0; // return waarde van de hardware is klaar

    //
    // eerst draaien naar 103
    //
    std::promise<void> done1;
    {
        InSequence in;
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(1,data.data); // relais 1 aan
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(0x3f,data.data); // richting inverted
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(3,data.data); // draai naar positie 3
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done1] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            done1.set_value();
            return 0;
        }));
    }

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(103));
    EXPECT_EQ(std::future_status::ready, done1.get_future().wait_for(std::chrono::seconds(1)));
    EXPECT_EQ(103,baanInfo->IOBits[0]->Stand);

    //
    // dan terug naar 0
    //

    std::promise<void> done2;
    {
        InSequence in;
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(48,data.data); // alle relais uit
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(0x3e,data.data); // richting normaal
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(0,data.data); // draai naar positie 0
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done2] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            done2.set_value();
            return 0;
        }));
    }

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(100));
    EXPECT_EQ(std::future_status::ready, done2.get_future().wait_for(std::chrono::seconds(1)));
    EXPECT_EQ(100,baanInfo->IOBits[0]->Stand);

    //
    // dan naar 203 (huisje aan de andere kant
    //
    std::promise<void> done3;
    {
        InSequence in;
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(1,data.data); // relais 1 aan
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(0x3e,data.data); // richting normaal
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(303,data.adres);
            EXPECT_EQ(27,data.data); // draai naar positie 3 andersom
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done3] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            done3.set_value();
            return 0;
        }));
    }

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(203));
    EXPECT_EQ(std::future_status::ready, done3.get_future().wait_for(std::chrono::seconds(1)));
    EXPECT_EQ(203,baanInfo->IOBits[0]->Stand);


    //
    // dan naar 200 (huisje aan de andere kant
    //
    std::promise<void> done4;
    {
        InSequence in;
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(48,data.data); // alle relais uit
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(301,data.adres);
            EXPECT_EQ(0x3f,data.data); // richting inverted
            return 0;
        }));
        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [] (const hardwareArray_t & data){
            EXPECT_EQ(303,data.adres);
            EXPECT_EQ(24,data.data); // draai naar positie 0 andersom
            return 0;
        }));

        EXPECT_CALL(mHardwareHoog,nieuwItem(_)).WillOnce(Invoke( [&done4] (const hardwareArray_t & data){
            EXPECT_EQ(302,data.adres);
            EXPECT_EQ(63,data.data); // get status
            done4.set_value();
            return 0;
        }));
    }

    EXPECT_EQ(0,baanInfo->IOBits[0]->Aanvraag(200));
    EXPECT_EQ(std::future_status::ready, done4.get_future().wait_for(std::chrono::seconds(1)));
    EXPECT_EQ(200,baanInfo->IOBits[0]->Stand);

}
