#ifndef IMAINWINDOWDRAWING_H
#define IMAINWINDOWDRAWING_H

#include <string>

class IMainWindowDrawing
{
public:
    enum BaanColor
    {
        Black = 0x000000,
        Red = 0xff0000,
        Green = 0x00ff00,
        Yellow = 0xffff00,
        Gray66 = 0x666666
    };

    virtual void SetBitmap(const std::string & bitmapName) = 0;
    virtual void RedrawAll() = 0;
    virtual void RedrawRec(int x,int y,int width, int height) = 0;

    // actie kan een van de volgende zijn BLOK_REMOVE, BLOK_CLEAR, BLOK_BELEG, BLOK_KOPBLOK, BLOK_KORTSLUIT, BLOK_RESERVED
    virtual void BlokDisplay(int regelaar, int actie, int xcoord, int ycoord, const std::string& regelBitmapName, int blokIONummer) = 0;
    virtual void BlokClearEditModeDisplay(bool blokSelected, int xcoord, int ycoord, const std::string& blokText) = 0;
    virtual void DisplayString(int x, int y, const std::string& value)=0;
    virtual void DrawLine(int fromx, int fromy, int tox, int toy, int dikte, BaanColor color) = 0;
    virtual void VierkantDisplay(int x,int y,int width,int height,BaanColor color) = 0;
};

#endif // IMAINWINDOWDRAWING_H
