#include "baanMessage.h"
#include "baanSemaphore.h"
#include "baanQueue.h"
#include "baan.h"
#include <stdio.h>
#include "td.h"


BaanMessage::BaanMessage(IViewInternal &viewInternal, ITd& td, std::vector<Regelaar>& regelaars):
    mViewInternal(viewInternal),
    mTd(td),
    mRegelaars(regelaars)
{
    running = 0;
    tdMessage = mTd.tdCreate("messageHandler");
    tdOnMessage = mTd.tdValueString ("OnMessage");
}

BaanMessage::~BaanMessage()
{
    Stop();
}

int BaanMessage::Post (int type, int value1, int value2, int value3)
{
    int buf[4];
    int ret;

    if (baanQueue)
    {
        buf[0] = type;
        buf[1] = value1;
        buf[2] = value2;
        buf[3] = value3;

        ret = baanQueueSend (baanQueue, buf);
    }
    return ret;
}

int BaanMessage::Start()
{
    // eerst controleren of de message handler nog loopt
    printf ("INIT %d\n", running);
    if (running)
        return 1;
    baanQueue = baanQueueCreate (100, "wtNaarHandler");
    if (baanQueue == NULL)
        return 2;
    stop = 0;
    workerThread = boost::thread(boost::bind(&BaanMessage::ThreadProc,this));
                                             //boost::ref(this)));
    running = 1;
    return 0;
}

void BaanMessage::Stop ()
{
    if (running)
    {
        stop = 1;
        Post (-1, -1, -1, -1);
        workerThread.join();
        baanQueueDestroy (baanQueue);
        running = 0;
    }
}

void BaanMessage::ThreadProc (void)

{
    while (stop == 0)
    {
        int buf[4];
        int count;
        baanQueueReceive (baanQueue, buf);
        //      printf ("Received message %d %d %d %d\n",
        //            buf[0], buf[1], buf[2], buf[3]);
        // handle the message
        if (stop == 0)
        {
            mTd.tdStart (tdMessage);
            do
            {
                mTd.tdValue (tdOnMessage, buf[0]);
                switch (buf[0])
                {
                case WM_ECHTE_SNELHEID:
                    mRegelaars[buf[1]].OnEchteSnelheid ();
                    break;
                case WM_GEEF_PROGRESS:
                    mRegelaars[buf[1]].SnelheidProgress(buf[2]);
                    break;
                case WM_BLOK_DISPLAY:
                    mViewInternal.Update(buf[0], buf[1], buf[2], buf[3]);
                    break;
                case WM_TIMER_VERLOPEN:
                case WM_BLOK_EVENT:
                case WM_INTERNALTIMER_VERLOPEN:
                case WM_VIERKANT_DISPLAY:
                    mViewInternal.Update(buf[0], buf[1], buf[2], buf[3]);
                    break;
                case WM_WISSEL_DISPLAY:
                    mViewInternal.Update(buf[0], buf[1], buf[2], buf[3]);
                    break;
                }
                count = baanQueueCount (baanQueue);
                if ((count) && (stop == 0))
                    baanQueueReceive (baanQueue, buf);

            }
            while ((stop == 0) && (count));

            mTd.tdStop (tdMessage);
        }
    }
    stop = 0;
    running = 0;
    return;
}
