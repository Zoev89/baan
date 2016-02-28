#ifndef _BAANMESSAGE_H_
#define _BAANMESSAGE_H_

#include "IBaanMessage.h"
#include "baanQueue.h"
#include <boost/thread.hpp>
#include "IViewInternal.h"
#include "ITd.h"
#include "baanTypes.h"
#include "regelaar.h"

class BaanMessage : public IBaanMessage
{
public:
    BaanMessage(IViewInternal& viewInternal, ITd& td, std::vector<Regelaar>& regelaars);
    ~BaanMessage();

    virtual int Post(int type, int value1, int value2,int value3);
    virtual int Start();
    virtual void Stop();

private:
    IViewInternal& mViewInternal;
    ITd& mTd;
    std::vector<Regelaar>& mRegelaars;
    void ThreadProc (void);
    pbaanQueue_t baanQueue;
    int stop;
    int running;
    boost::thread workerThread;
    int tdMessage;
    int tdOnMessage;

};


#endif // _BAANMESSAGE_H_

