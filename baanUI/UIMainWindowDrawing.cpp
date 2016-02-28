#include "UIMainWindowDrawing.h"
#include <QPaintEvent>
#include <iostream>

UIMainWindowDrawing::UIMainWindowDrawing(QWidget *parent):
    ::QWidget(parent)

{

}

void UIMainWindowDrawing::SetBitmap(const std::string & bitmapName)
{
    emit SetBitmapSignal(QString(bitmapName.c_str()));
}

void UIMainWindowDrawing::RedrawAll()
{
    //std::cout << "redrawall sig" << std::endl;
    emit RedrawAllSignal();
}

void UIMainWindowDrawing::RedrawRec(int x, int y, int width, int height)
{
    emit RedrawRecSignal(x,y,width,height);
}

void UIMainWindowDrawing::BlokDisplay(int regelaar, int actie, int xcoord, int ycoord, const std::string &regelBitmapName, int blokIONummer)
{
    emit BlokDisplaySignal(regelaar, actie, xcoord, ycoord, QString(regelBitmapName.c_str()), blokIONummer);

    //QMetaObject::invokeMethod(this,"BlokDisplaySignal", Q_ARG(int,regelaar),  Q_ARG(int,actie), Q_ARG(int,xcoord), Q_ARG(int,ycoord),
//            Q_ARG(QString,QString(regelBitmapName.c_str())),Q_ARG(int, blokIONummer));
}

void UIMainWindowDrawing::BlokClearEditModeDisplay(bool blokSelected, int xcoord, int ycoord, const std::string &blokText)
{
    //QMetaObject::invokeMethod(this,"BlokClearEditModeDisplaySignal", Q_ARG(int,regelaar), Q_ARG(bool,blokSelected), Q_ARG(int,xcoord), Q_ARG(int,ycoord),
//            Q_ARG(QString,QString(regelBitmapName.c_str())), Q_ARG(QString,QString(blokText.c_str())));

    emit BlokClearEditModeDisplaySignal(blokSelected, xcoord, ycoord, QString(blokText.c_str()));
}

void UIMainWindowDrawing::DisplayString(int x, int y, const std::string &value)
{
    emit DisplayStringSignal(x, y, QString(value.c_str()));
}

void UIMainWindowDrawing::DrawLine(int fromx, int fromy, int tox, int toy, int dikte, IMainWindowDrawing::BaanColor color)
{
    emit DrawLineSignal(fromx,fromy, tox,toy,dikte,color);

}

void UIMainWindowDrawing::VierkantDisplay(int x, int y, int width, int height, BaanColor color)
{
    emit VierkantDisplaySignal(x, y, width, height, color);
}

