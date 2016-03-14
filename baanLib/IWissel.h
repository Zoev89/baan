#ifndef IWISSEL_H
#define IWISSEL_H
#include <stdio.h>
#include <functional>
#include <string>

struct BlokPointer_t;

class IWissel
{
public:

    virtual int Init(const char *Input, std::function<std::string()> newInput) = 0;
    virtual void InitRoutering ()=0;
    virtual void ZoekOngeinitializeerdeEindBlokken()=0;
    virtual void Display ()=0;
    virtual int Aanvraag(int stand)=0;
    virtual void String(char *string)=0;
    virtual void NieuwXY (int selectionX, int selectionY, int deltaX, int deltaY)=0;
    virtual void InitDialoog ()=0;
    virtual void DialoogOk ()=0;
    virtual void TestIOTimer ()=0;
    virtual float GetAdres() = 0;
    virtual int CheckWissel() = 0;
    virtual void VeranderBlok (BlokPointer_t * oudBlok, BlokPointer_t * nieuwBlok) = 0;
    virtual BlokPointer_t * GetWisselBlok() = 0;
    virtual int GetRichting() = 0;
    virtual int WisselLengte(int from, int to) = 0;

protected:
    virtual ~IWissel() {}


};


#endif // IWISSEL_H
