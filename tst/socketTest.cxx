#include "gtest/gtest.h"
#include "../src/socketCom.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class Semaphore
{
private:
    boost::mutex mutex_;
    boost::condition_variable condition_;
    unsigned long count_;

public:
    Semaphore()
        : count_()
    {}

    void notify()
    {
        boost::mutex::scoped_lock lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void wait()
    {
        boost::mutex::scoped_lock lock(mutex_);
        while(!count_)
            condition_.wait(lock);
        --count_;
    }
    
    bool wait(boost::posix_time::time_duration const wait_duration)
    {
    
        bool retval;
        
        boost::mutex::scoped_lock lock(mutex_);
        if(condition_.timed_wait(lock,wait_duration,boost::bind(&Semaphore::CondFulfilled, this)))
        {
            --count_;
            retval = true;
        }
        else
        {
            retval =false;
        }
        return retval;
    }
    bool CondFulfilled() { return count_!=0; }
};

class SocketTest : public ::testing::Test {
 protected:
    virtual void SetUp() {
    }

    virtual void TearDown() 
    {}

public:
    void callBack(int index, int intData, void *data)
    {
        printf("callback %d data %d\n",index,intData);
        m_index = index;
        m_intData = intData;
        m_receiveSema.notify();
    }
protected:
    int m_index;
    int m_intData;
    Semaphore m_receiveSema;
  
};

TEST_F(SocketTest, ConstructorTest)
{
    SocketServerCom server("5000");
}

TEST_F(SocketTest, connectDisconnect)
{
    SocketServerCom server("5000");
    server.Connect();
    ASSERT_FALSE(server.GetConnected());
    server.Disconnect();
}


TEST_F(SocketTest, sendDataAndRecieveOnEmpty)
{
    SocketServerCom server("5000");
    server.Connect();
    system("xterm -e ./client localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarSnelheid(15);
        server.Send(data);
    }
    sleep(1);
    ASSERT_TRUE(server.GetConnected());
}

TEST_F(SocketTest, zetSnelheid)
{
    SocketServerCom server("5000",boost::bind( &SocketTest::callBack, this, _1, _2, _3) );
    server.Connect();
    system("xterm -e ./client localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarSnelheid(15);
        server.Send(data);
    }
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(2000)));
    ASSERT_EQ(SocketData::SETACTIVEREGELAARSNELHEID, m_index);
    ASSERT_EQ(15, m_intData);
}

TEST_F(SocketTest, zetRichting)
{
    SocketServerCom server("5000",boost::bind( &SocketTest::callBack, this, _1, _2, _3) );
    server.Connect();
    system("xterm -e ./client localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarRichting(SocketData::VOORUIT);
        server.Send(data);
    }
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(2000)));
    ASSERT_EQ(SocketData::SETACTIVEREGELAARRICHTING, m_index);
    ASSERT_EQ((int)SocketData::VOORUIT, m_intData);
}

TEST_F(SocketTest, zetRichtingEnSnelheid)
{
    SocketServerCom server("5000",boost::bind( &SocketTest::callBack, this, _1, _2, _3) );
    server.Connect();
    system("xterm -e ./client localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarRichting(SocketData::VOORUIT);
        data.SetActiveRegelaarSnelheid(13);
        server.Send(data);
    }
    // richting info gaat verlore maar ik verwacht wel dat die 2maal aangeroepen is
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(2000)));
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(2000)));
    ASSERT_EQ(SocketData::SETACTIVEREGELAARRICHTING, m_index);
    ASSERT_EQ(SocketData::VOORUIT, m_intData);
}

TEST_F(SocketTest, clientClose)
{
    SocketServerCom server("5000",boost::bind( &SocketTest::callBack, this, _1, _2, _3) );
    server.Connect();
    system("xterm -e ./clientEnkel localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarRichting(SocketData::VOORUIT);
        server.Send(data);
    }
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(2000)));
    ASSERT_EQ(SocketData::SETACTIVEREGELAARRICHTING, m_index);
    ASSERT_EQ((int)SocketData::VOORUIT, m_intData);
    sleep(1);
    ASSERT_FALSE(server.GetConnected());
    system("xterm -e ./clientEnkel localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarRichting(SocketData::ACHTERUIT);
        server.Send(data);
    }
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(2000)));
    ASSERT_EQ(SocketData::SETACTIVEREGELAARRICHTING, m_index);
    ASSERT_EQ((int)SocketData::ACHTERUIT, m_intData);
}

TEST_F(SocketTest, zetIetsAlsNietConnected)
{
    SocketServerCom server("5000",boost::bind( &SocketTest::callBack, this, _1, _2, _3) );
    server.Connect();
    {
        m_intData = 10;
        SocketData data;
        data.SetActiveRegelaarSnelheid(9);
        server.Send(data);
    }
    ASSERT_FALSE(m_receiveSema.wait(boost::posix_time::milliseconds(1000)));
    ASSERT_EQ(10, m_intData);
    system("xterm -e ./client localhost 5000 &");
    {
        SocketData data;
        data.SetActiveRegelaarSnelheid(11);
        server.Send(data);
    }
    sleep(1);
    ASSERT_TRUE(m_receiveSema.wait(boost::posix_time::milliseconds(1000)));
    ASSERT_EQ(SocketData::SETACTIVEREGELAARSNELHEID, m_index);
    ASSERT_EQ(11, m_intData);
}

TEST_F(SocketTest, convertToChar)
{
    SocketData data;
    char ret;
    int i;
    for(i=0;i<27;i++)
    {
        ret = data.ConvertToChar(i);
        ASSERT_EQ(i+'@', ret);
    }
    for(;i<32;i++)
    {
        ret = data.ConvertToChar(i);
        ASSERT_EQ(i+21, ret);
    }
    for(;i<59;i++)
    {
        ret = data.ConvertToChar(i);
        ASSERT_EQ(i+'`'-32, ret);
    }
    for(;i<64;i++)
    {
        ret = data.ConvertToChar(i);
        ASSERT_EQ(i-6, ret);
    }
}

TEST_F(SocketTest, zendBitmap)
{
    SocketServerCom server("5000",boost::bind( &SocketTest::callBack, this, _1, _2, _3) );
    server.Connect();
    system("xterm -e ./client localhost 5000 &");
    int w=20;
    int h=20;
    int s=w*4;
    unsigned int argb[h*w];
    {
        for(int i=0;i<(w*h);i++)
        {
            int r,g,b;
            r=(i*3) & 0xff;
            g=(i*3+1) & 0xff;
            b=(i*3+2) & 0xff;
            argb[i] = i; //(r<<16) + (g<<8) + b;
        }
        SocketData data;
        data.SendNamedBitmap(argb,w,h,s,"aap",0);
        server.Send(data);
    }
    sleep(1);
}
