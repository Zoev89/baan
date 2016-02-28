#include "gtest/gtest.h"
#include "AllMocks.h"
#include "td.h"

using ::testing::_;

class tdTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(tdTest, Constructie)
{
    Td td(2);
}

TEST_F(tdTest, TestMaxTasks)
{
    Td td(2);

    for (std::size_t x=0;x<td.TD_MAX_NAMES;x++)
    {
        EXPECT_GT(td.TD_MAX_NAMES, (std::size_t)td.tdCreate("aap"));
    }
    // expect dat die faalt
    EXPECT_EQ(-1, td.tdCreate("aap"));
}

TEST_F(tdTest, TestMaxValueStrings)
{
    Td td(2);

    for (std::size_t x=0;x<td.TD_MAX_NAMES;x++)
    {
        EXPECT_GT(td.TD_MAX_NAMES, (std::size_t)td.tdValueString("aap"));
    }
    // expect dat die faalt
    EXPECT_EQ(-1, td.tdValueString("aap"));
}
