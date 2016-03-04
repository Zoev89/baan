#ifndef QBITMAPLABEL_H
#define QBITMAPLABEL_H
#include "QLabel"
#include "QWidget"
#include "QPixmap"
#include "UIRegelaars.h"
#include "QPainter"

#include "baanLib.h"
//
// QGraphicsView kan misschien interresant zijn
//

class QBitmapLabel : public QLabel
{
    Q_OBJECT

    struct drawLine
    {
        int fromx;
        int fromy;
        int tox;
        int toy;
        int dikte;
        int color;
    };

public:
    QBitmapLabel(QWidget * parent, BaanLib * baanLib, UIRegelaars *regelaars);
protected:
    void paintEvent( QPaintEvent * event );
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    virtual void keyPressEvent(QKeyEvent * event);


public Q_SLOTS:
    void DrawLineSlot(int fromx, int fromy, int tox, int toy, int dikte, int color);
    void RedrawAllSlot();
    void SetBitmapSlot(const QString bitmapName);
    void RedrawRecSlot(int x, int y, int width, int height);
    void BlokDisplaySlot(int regelaar, int actie, int xcoord, int ycoord, const QString regelBitmapName, int blokIONummer);
    void BlokClearEditModeDisplaySlot(bool blokSelected, int xcoord, int ycoord, const QString blokText);
    void DisplayStringSlot(int x, int y, const QString value);
    void VierkantDisplaySlot(int x, int y, int width, int height, int color);
private:
    BaanLib * mBaanLib;
    UIRegelaars *mRegelaars;

    std::unique_ptr<QPixmap> mInputBitmap;
    QPixmap mWorkingBitmap;
};

#endif // QBITMAPLABEL_H
