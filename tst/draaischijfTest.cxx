
#include "gtest/gtest.h"
#include "baanLibSetup.h"
//using ::testing::_;
using ::testing::Return;
using ::testing::_;

class draaischijfTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        baanInfo = new BaanInfo_t;
        objects =  new InitObjects(baanInfo); // Program Init fails want de objects zijn niet geinitializeerd bij mBaanInfo new hier boven
    }

    virtual void TearDown()
    {
    }

    InitObjects *objects;
    BaanInfo_t* baanInfo = NULL;

};

TEST_F(draaischijfTest, Constructie)
{
    objects->wissels.CreateNewIO(7);
    EXPECT_EQ(0,objects->wissels.Init(0,"7 300.0 10 12 40",[]()
    {
        std::string ret;
        return ret;
    }));
    char string[100];
    objects->wissels.String(0, string);
    std::string output(string);

    EXPECT_EQ("7  300.00   10   12   40",output);
}
