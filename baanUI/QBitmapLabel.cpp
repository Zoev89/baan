#include "QBitmapLabel.h"
#include "QString"
#include "QRect"
#include <QPaintEvent>
#include <iostream>

QBitmapLabel::QBitmapLabel(QWidget * parent, BaanLib *baanLib, UIRegelaars *regelaars) :
    ::QLabel(parent)
    , mBaanLib(baanLib)
    , mRegelaars(regelaars)
{
   setFocusPolicy(Qt::ClickFocus);
}

void QBitmapLabel::paintEvent ( QPaintEvent * /*event*/ )
{
//printf("paint\n");
    QPainter painter(this);
    painter.drawPixmap(mWorkingBitmap.rect(), mWorkingBitmap);

    //QLabel::paintEvent(event);
}

void QBitmapLabel::mouseMoveEvent(QMouseEvent *ev)
{
    //std::cout << "Mouse move " << ev->button() << " b " << ev->buttons()<< " "<< ev->x() << "," << ev->y() << std::endl;
    if (ev->buttons() == Qt::LeftButton)
    {
        mBaanLib->HandleMouseEvents (IView::ButtonDrag, IView::LeftButton ,ev->x(), ev->y());
    }
    else if (ev->buttons() == Qt::RightButton)
    {
        mBaanLib->HandleMouseEvents (IView::ButtonDrag, IView::RightButton ,ev->x(), ev->y());
    }

}

void QBitmapLabel::mousePressEvent(QMouseEvent *ev)
{
//QLabel::mousePressEvent(ev);
//std::cout << "Mouse event " << ev->button() << " "<< ev->x() << "," << ev->y() << std::endl;

    if (ev->button() == Qt::LeftButton)
    {
        mBaanLib->HandleMouseEvents (IView::ButtonClick, IView::LeftButton ,ev->x(), ev->y());
    }
    if (ev->button() == Qt::RightButton)
    {
        mBaanLib->HandleMouseEvents (IView::ButtonClick, IView::RightButton ,ev->x(), ev->y());
    }
}

void QBitmapLabel::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
        mBaanLib->HandleKeyBoardEvent(IView::DeleteKey);
    else
        QLabel::keyPressEvent(event);

}

void QBitmapLabel::SetBitmapSlot(const QString bitmapName)
{
    //printf("SetBitmapSlot %s\n",bitmapName.toStdString().c_str());
    mInputBitmap.reset(new QPixmap(bitmapName));
    mWorkingBitmap = mInputBitmap->copy();
    setMinimumSize(mWorkingBitmap.width(), mWorkingBitmap.height());

    update();
}


void QBitmapLabel::RedrawAllSlot()
{
    //std::cout << "RedrawAllSlot" << std::endl;
    mWorkingBitmap = *mInputBitmap;
    mBaanLib->Draw();
    update();
}


void QBitmapLabel::DrawLineSlot(int fromx, int fromy, int tox, int toy, int dikte, int color)
{
    //printf("DrawLineSlot %d %d %d %d %d %x\n",fromx,fromy,tox,toy,dikte,color);

    QPainter painter(&mWorkingBitmap);
    QPen pen(Qt::black);
    pen.setWidth(dikte);
    QColor pencolor((color & 0xff0000)>>16, (color & 0xff00) >> 8, color & 0xff);
    pen.setColor(pencolor);
    painter.setPen(pen);
    painter.drawLine(fromx, fromy, tox, toy);
    update();
}

void QBitmapLabel::RedrawRecSlot(int x, int y, int width, int height)
{
    //printf("RedrawRecSlot %d %d %d %d\n",x,y,width,height);
    QPainter painter(&mWorkingBitmap);
    QRect rec(x, y, width, height);
    painter.drawPixmap(rec, mInputBitmap->copy(rec));
    update();
}

void QBitmapLabel::BlokDisplaySlot(int regelaar, int actie, int xcoord, int ycoord, const QString regelBitmapName, int blokIONummer)
{
    //printf("BlokDisplaySlot %d %d\n",regelaar,actie);
    // actie kan een van de volgende zijn BLOK_REMOVE, BLOK_CLEAR, BLOK_BELEG, BLOK_KOPBLOK, BLOK_KORTSLUIT, BLOK_RESERVED
    QPixmap *regelBitmap=nullptr;
    QRect rec(xcoord, ycoord, 38, 18); // default size
    if (regelaar >= 0)
    {
        regelBitmap = mRegelaars->GetBitmap(regelaar);
        rec.setWidth(regelBitmap->width());
        rec.setHeight(regelBitmap->height());
    }
    QPainter painter(&mWorkingBitmap);
    switch (actie)
    {
    case BLOK_BELEG:
    case BLOK_KORTSLUIT:
        painter.drawPixmap(rec, regelBitmap->copy());
        if (actie == BLOK_KORTSLUIT)
        {
            QPen pen(Qt::red);
            pen.setWidth(4);
            painter.setPen(pen);
            painter.drawLine(xcoord, ycoord, xcoord + regelBitmap->width(), ycoord + regelBitmap->height());
        }
        break;
    case BLOK_REMOVE:
    case BLOK_CLEAR:
        painter.drawPixmap(rec, mInputBitmap->copy(rec));
        if (actie == BLOK_CLEAR)
        {
            QPen pen(Qt::black);
            pen.setWidth(1);
            painter.setPen(pen);
            QString str;
            painter.drawText(rec, Qt::AlignCenter, str.setNum(blokIONummer));
        }

        break;
    case BLOK_KOPBLOK:
        rec.setWidth(rec.width()/2);
        painter.drawPixmap(rec, regelBitmap->copy(QRect(0, 0, rec.width(), rec.height())));
        break;
    case BLOK_RESERVED:
        printf("BlOK RESERVED Display???\n");
        break;
    default:
        printf("Invalid BlokDisplaySlot\n");
        break;

    }
    //std::cout << "disp " << regelBitmapName.toStdString() << " blok " << blokIONummer<< " reg " <<  regelaar<< std::endl;
    update();
}

void QBitmapLabel::BlokClearEditModeDisplaySlot(bool blokSelected, int xcoord, int ycoord, const QString blokText)
{
    QRect rec(xcoord-10, ycoord, 38+30, 12); // default size
    QPainter painter(&mWorkingBitmap);
    painter.drawPixmap(rec, mInputBitmap->copy(rec)); // clear the area


    QPen pen;
    if (blokSelected)
    {
        pen.setColor(Qt::red);
    }
    else
    {
        pen.setColor(Qt::black);
    }
    painter.setPen(pen);
    QFont font;
    font.setPixelSize(11);
    font.setWeight(QFont::Bold);
    painter.setFont(font);
    painter.drawText(rec, Qt::AlignCenter, blokText);
    update(); // make sure that it is drawn when I leave

}

void QBitmapLabel::DisplayStringSlot(int x, int y, const QString value)
{
    QPainter painter(&mWorkingBitmap);
    QPen pen(Qt::green);

    QFont font;
    font.setPixelSize(11);
    QFontMetrics fm(font);
    auto rec = fm.boundingRect(value);
    rec.moveLeft(x);
    rec.moveTop(y);

    painter.setFont(font);

    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawText(rec,  Qt::AlignLeft | Qt::AlignBottom, value);
    update(); // make sure that it is drawn when I leave
}

void QBitmapLabel::VierkantDisplaySlot(int x,int y,int width,int height,int color)
{
    QPainter painter(&mWorkingBitmap);
    // (rgbcolor>>16) & 0xff, (rgbcolor>>8) & 0xff, rgbcolor & 0xff )
    painter.fillRect( QRectF(x, y, width, height), QBrush(QColor(QRgb(color))));
    update(); // make sure that it is drawn when I leave
}
