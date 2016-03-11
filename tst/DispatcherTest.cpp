
#include "gtest/gtest.h"
#include "Dispatcher.h"
#include <future>
#include <chrono>

class dispatcherTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(dispatcherTest, ConstructieDestructie)
{
    Dispatcher disp;

}

TEST_F(dispatcherTest, Dispatch)
{
    Dispatcher disp;
    std::promise<void> p;
    auto fut = p.get_future();
    disp.Dispatch([&](){p.set_value();});
    EXPECT_EQ(std::future_status::ready, fut.wait_for(std::chrono::seconds(1)));
}
