#include "gtest/gtest.h"
#include "../src/spoorInformatie.h"
#include "../shared/programDefines.h"

class SpoorInformatieTest : public ::testing::Test {
 protected:
    virtual void SetUp() {
    }

    virtual void TearDown() 
    {
    }

public:
protected:
  
};

TEST_F(SpoorInformatieTest, ConstructorTest)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddStation("Waalre");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddSpoor("Eindhoven","Spoor 2");
    spoor.AddBlok("Eindhoven","Spoor 1",10);
    spoor.SaveData("baan");
    ASSERT_FALSE(spoor.IsBlokAanwezig("Eindhoven","Spoor 1",11));
    ASSERT_TRUE(spoor.IsBlokAanwezig("Eindhoven","Spoor 1",10));
}


TEST_F(SpoorInformatieTest, BlokAanwezigTest)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",10);
    ASSERT_FALSE(spoor.IsBlokAanwezig("Eindhoven","Spoor 1",11));
    ASSERT_TRUE(spoor.IsBlokAanwezig("Eindhoven","Spoor 1",10));
}

TEST_F(SpoorInformatieTest, SaveAndLoad)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",10);
    spoor.SaveData("baan");
    SpoorInformatie spoorNew;
    spoorNew.LoadData("baan");
    ASSERT_FALSE(spoorNew.IsBlokAanwezig("Eindhoven","Spoor 1",11));
    ASSERT_TRUE(spoorNew.IsBlokAanwezig("Eindhoven","Spoor 1",10));
}

TEST_F(SpoorInformatieTest, LoadMetFouteNaam)
{
    SpoorInformatie spoor;
    spoor.LoadData("baan1");
    ASSERT_FALSE(spoor.IsBlokAanwezig("Eindhoven","Spoor 1",10));
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",10);
    ASSERT_TRUE(spoor.IsBlokAanwezig("Eindhoven","Spoor 1",10));
}

TEST_F(SpoorInformatieTest, AddStopBlok)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",11);
    spoor.AddBlok("Eindhoven","Spoor 1",12);
    spoor.AddBlok("Eindhoven","Spoor 1",13);
    spoor.AddStopBlok("Eindhoven","Spoor 1",14,RICHTING_VOORUIT);
    spoor.AddStopBlok("Eindhoven","Spoor 1",10,RICHTING_ACHTERUIT);
    EXPECT_EQ(14, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(10, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
    EXPECT_EQ(13, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(11, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
}

TEST_F(SpoorInformatieTest, AddStopBlokVerkeerd)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",11);
    spoor.AddStopBlok("Eindhoven","Spoor 1",10,RICHTING_VOORUIT);
    EXPECT_EQ(10, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(11, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
    EXPECT_EQ(11, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(10, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
}

TEST_F(SpoorInformatieTest, AddGetStopBlokDefault)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",10);
    spoor.AddBlok("Eindhoven","Spoor 1",11);
    EXPECT_EQ(11, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(10, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(10, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
    EXPECT_EQ(11, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
}

TEST_F(SpoorInformatieTest, AddBlok1BlokTotaal)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddStopBlok("Eindhoven","Spoor 1",10,RICHTING_VOORUIT);
    EXPECT_EQ(10, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(10, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
    EXPECT_EQ(10, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(10, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
}

TEST_F(SpoorInformatieTest, AddStopBlok1BlokTotaal)
{
    SpoorInformatie spoor;
    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddBlok("Eindhoven","Spoor 1",11);
    EXPECT_EQ(11, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(11, spoor.GetStopBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
    EXPECT_EQ(11, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_VOORUIT));
    EXPECT_EQ(11, spoor.GetTreinBlok("Eindhoven","Spoor 1",RICHTING_ACHTERUIT));
}

TEST_F(SpoorInformatieTest, GenereerBaan)
{
    SpoorInformatie spoor;
    spoor.AddStation("Berlijn");
    spoor.AddSpoor("Berlijn","Spoor 1");
    spoor.AddBlok("Berlijn","Spoor 1",84);
    spoor.AddBlok("Berlijn","Spoor 1",67);
    spoor.AddBlok("Berlijn","Spoor 1",80);
    spoor.AddStopBlok("Berlijn","Spoor 1",54,RICHTING_VOORUIT);
    
    spoor.AddSpoor("Berlijn","Spoor 2");
    spoor.AddStopBlok("Berlijn","Spoor 2",87,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor 2",83);
    spoor.AddBlok("Berlijn","Spoor 2",68);
    spoor.AddBlok("Berlijn","Spoor 2",52);
    
    spoor.AddSpoor("Berlijn","Spoor 3");
    spoor.AddStopBlok("Berlijn","Spoor 3",89,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor 3",69);
    spoor.AddStopBlok("Berlijn","Spoor 3",49,RICHTING_VOORUIT);
    
    spoor.AddSpoor("Berlijn","Spoor 4");
    spoor.AddStopBlok("Berlijn","Spoor 4",90,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor 4",70);
    spoor.AddStopBlok("Berlijn","Spoor 4",50,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor 5");
    spoor.AddStopBlok("Berlijn","Spoor 5",91,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor 5",71);
    spoor.AddStopBlok("Berlijn","Spoor 5",51,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor 6");
    spoor.AddBlok("Berlijn","Spoor 6",82);
    spoor.AddBlok("Berlijn","Spoor 6",72);
    spoor.AddBlok("Berlijn","Spoor 6",58);
    spoor.AddStopBlok("Berlijn","Spoor 6",57,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor 7");
    spoor.AddStopBlok("Berlijn","Spoor 7",95,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor 7",81);
    spoor.AddBlok("Berlijn","Spoor 7",73);
    spoor.AddBlok("Berlijn","Spoor 7",60);

    spoor.AddSpoor("Berlijn","Spoor R1");
    spoor.AddStopBlok("Berlijn","Spoor R1",97,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor R1",74);
    spoor.AddStopBlok("Berlijn","Spoor R1",61,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor R2");
    spoor.AddStopBlok("Berlijn","Spoor R2",79,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor R2",75);
    spoor.AddStopBlok("Berlijn","Spoor R2",77,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor R3");
    spoor.AddStopBlok("Berlijn","Spoor R3",78,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor R3",76);
    spoor.AddStopBlok("Berlijn","Spoor R3",65,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor E1");
    spoor.AddBlok("Berlijn","Spoor E1",66);
    spoor.AddSpoor("Berlijn","Spoor E2");
    spoor.AddStopBlok("Berlijn","Spoor E2",63,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor E2",53);
    spoor.AddStopBlok("Berlijn","Spoor E2",120,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor G1");
    spoor.AddStopBlok("Berlijn","Spoor G1",118,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor G1",123);
    spoor.AddStopBlok("Berlijn","Spoor G1",124,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor G2");
    spoor.AddStopBlok("Berlijn","Spoor G2",119,RICHTING_ACHTERUIT);
    spoor.AddBlok("Berlijn","Spoor G2",121);
    spoor.AddStopBlok("Berlijn","Spoor G2",122,RICHTING_VOORUIT);

    spoor.AddSpoor("Berlijn","Spoor G3");
    spoor.AddBlok("Berlijn","Spoor G3",126);

    spoor.AddSpoor("Berlijn","Spoor T1");
    spoor.AddBlok("Berlijn","Spoor T1",103);
    spoor.AddSpoor("Berlijn","Spoor T2");
    spoor.AddBlok("Berlijn","Spoor T2",104);
    spoor.AddSpoor("Berlijn","Spoor T3");
    spoor.AddBlok("Berlijn","Spoor T3",105);
    spoor.AddSpoor("Berlijn","SpoorTram");
    spoor.AddBlok("Berlijn","SpoorTram",99);

    spoor.AddStation("Eindhoven");
    spoor.AddSpoor("Eindhoven","Spoor 1");
    spoor.AddStopBlok("Eindhoven","Spoor 1",22,RICHTING_ACHTERUIT);
    spoor.AddBlok("Eindhoven","Spoor 1",1);

    spoor.AddSpoor("Eindhoven","Spoor 2");
    spoor.AddStopBlok("Eindhoven","Spoor 2",23,RICHTING_ACHTERUIT);
    spoor.AddBlok("Eindhoven","Spoor 2",2);

    spoor.AddSpoor("Eindhoven","Spoor 3");
    spoor.AddBlok("Eindhoven","Spoor 3",3);
    spoor.AddStopBlok("Eindhoven","Spoor 3",4,RICHTING_VOORUIT);

    spoor.AddSpoor("Eindhoven","Spoor 4");
    spoor.AddBlok("Eindhoven","Spoor 4",5);
    spoor.AddStopBlok("Eindhoven","Spoor 4",6,RICHTING_VOORUIT);
    
    spoor.AddStation("Waalre");
    spoor.AddSpoor("Waalre","Spoor 1");
    spoor.AddStopBlok("Waalre","Spoor 1",26,RICHTING_ACHTERUIT);
    spoor.AddBlok("Waalre","Spoor 1",32);
    spoor.AddStopBlok("Waalre","Spoor 1",36,RICHTING_VOORUIT);

    spoor.AddSpoor("Waalre","Spoor 2");
    spoor.AddStopBlok("Waalre","Spoor 2",40,RICHTING_ACHTERUIT);
    spoor.AddBlok("Waalre","Spoor 2",31);
    spoor.AddStopBlok("Waalre","Spoor 2",24,RICHTING_VOORUIT);

    spoor.AddStation("WaalreOnder");
    spoor.AddSpoor("WaalreOnder","SpoorDoor");
    spoor.AddBlok("WaalreOnder","SpoorDoor",19);
    spoor.AddBlok("WaalreOnder","SpoorDoor",29);

    spoor.AddSpoor("WaalreOnder","SpoorDood");
    spoor.AddBlok("WaalreOnder","SpoorDood",42);

    spoor.AddSpoor("WaalreOnder","SpoorTransport");
    spoor.AddBlok("WaalreOnder","SpoorTransport",15);

spoor.SaveData("mijnbaan");
}
