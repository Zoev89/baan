#ifndef IBAANWT_H
#define IBAANWT_H

class IBaanWT
{
public:
    virtual void InitWorkThread (void)=0;
    virtual void StartWorker(void)=0;
    virtual void StopWorker(void)=0;
    virtual void ericSleep (int tijdInMs) = 0;
    virtual void BaanCheckLengte (int RegelNummer, int NeemKopBlokMee) = 0;
};

#endif // IBAANWT_H
