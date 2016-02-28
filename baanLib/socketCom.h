#pragma once

#include "string"
#include <boost/thread.hpp>
#include "concurrentQueue.h"

using namespace std;

typedef boost::function<void(int index , int intData, void *data)> callBack;

class SocketData
{
public:
    SocketData();
    void SetActiveRegelaarSnelheid(int waarde);
    int GetActiveRegelaarSnelheid();
    enum Richting
    {
        VOORUIT,
        ACHTERUIT
    };
    void SetActiveRegelaarRichting(Richting voorTerug);
    enum CallBackData
    {
        SETACTIVEREGELAARSNELHEID,
        SETACTIVEREGELAARRICHTING,
        SENDBITMAP,
        SENDBITMAPINTERNALDATA,
        LASTELEMENT
    };
    void SetStopProcessing();
    bool GetStopProcessing();
    string GetStrings();
    void ProcessReceivedBuffer(char *buf, callBack cb);
    void SendNamedBitmap(unsigned int *argb, int width, int height, int stride, string name, int index);
    char ConvertToChar(int value); // for testing public
    
private:
    bool findIntergerValue(char *buf,int *value);
    bool m_whichData[LASTELEMENT];
    bool m_stopProcessing;
    int m_activeSnelheid;
    Richting m_activeRichting;
    int m_index;
    unsigned int *m_argb;
    int m_width;
    int m_height;
    int m_stride;
    string m_bitmapName;
    int m_bitmapIndex;
    string ConvertARGBToString(unsigned int argb);
    int m_pixelPosition;
    int m_line;
};

class SocketServerCom
{
public:
    SocketServerCom(string socketPoort, callBack cb=NULL);
    ~SocketServerCom();
    void Connect(void);
    void Disconnect(void);
    void Send(SocketData &data);
    bool GetConnected(void);
    
private:
    void ReadChannel(void);
    void WriteChannel(void);
    const string m_socketPoort;
    boost::thread m_read;
    boost::thread m_write;
    bool m_stop;
    bool m_close;
    bool m_connected;
    int m_socket;
    int m_socketChannel;
    ConcurrentQueue<SocketData> m_queue;
    callBack m_callBack;
};

