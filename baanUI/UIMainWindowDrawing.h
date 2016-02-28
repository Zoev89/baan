#ifndef UIMAINWINDOWDRAWING_H
#define UIMAINWINDOWDRAWING_H
#include "IMainWindowDrawing.h"
#include "QString"
#include "QWidget"

class UIMainWindowDrawing :  public QWidget, public IMainWindowDrawing
{
    Q_OBJECT
public:

    UIMainWindowDrawing(QWidget * parent);
    // IMainWindowDrawing
    virtual void SetBitmap(const std::string & bitmapName);
    virtual void RedrawAll();
    virtual void RedrawRec(int x,int y,int width, int height);
    virtual void BlokDisplay(int regelaar, int actie, int xcoord, int ycoord, const std::string& regelBitmapName, int blokIONummer);
    virtual void BlokClearEditModeDisplay(bool blokSelected, int xcoord, int ycoord, const std::string& blokText);
    virtual void DisplayString(int x, int y, const std::string& value);
    virtual void DrawLine(int fromx, int fromy, int tox, int toy, int dikte, BaanColor color);
    virtual void VierkantDisplay(int x,int y,int width,int height,BaanColor color);

Q_SIGNALS:
    void DrawLineSignal(int fromx, int fromy, int tox, int toy, int dikte, int color);
    void RedrawAllSignal();
    void SetBitmapSignal(const QString bitmapName);
    void RedrawRecSignal(int x, int y, int width, int height);
    void BlokDisplaySignal(int regelaar, int actie, int xcoord, int ycoord, const QString regelBitmapName, int blokIONummer);
    void BlokClearEditModeDisplaySignal(bool blokSelected, int xcoord, int ycoord, const QString blokText);
    void DisplayStringSignal(int x, int y, const QString value);
    void VierkantDisplaySignal(int x,int y,int width,int height,int color);
private:
};

#endif // UIMAINWINDOWDRAWING_H
