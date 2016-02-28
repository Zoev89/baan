#ifndef IBAANMESSAGE_H
#define IBAANMESSAGE_H

class IBaanMessage
{
public:
    virtual int Post(int type, int value1, int value2,int value3) = 0;
    virtual int Start() = 0;
    virtual void Stop() = 0;
};

#endif // IBAANMESSAGE_H
