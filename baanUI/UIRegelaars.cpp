#include "UIRegelaars.h"

UIRegelaars::UIRegelaars(QWidget *parent, BaanLib *baanLib) :
    QWidget(parent),
    mBaanLib(baanLib),
    mActiveRegelaar(0)
{
    mRegelaar = new UIRegelaar(this);
}

QPixmap* UIRegelaars::GetBitmap(int regelaarNummer)
{
    return mRegelaars[regelaarNummer].bitmap;
}


void UIRegelaars::NieuweRegelaarSlot(int regelaarNummer, const QString bitmapFileName)
{
    //std::cout << "reg " << regelaarNummer << "  " << bitmapFileName.toStdString() << std::endl;
    mRegelaars[regelaarNummer].bitmap = new QPixmap(bitmapFileName);
}

void UIRegelaars::SnelheidProgressSlot(int regelaarNummer, int snelheid)
{
    mRegelaars[regelaarNummer].snelheidProgess = snelheid;
    updateRegelaar(regelaarNummer);
}

void UIRegelaars::MeldingenSlot(int regelaarNummer, const QString meldingText)
{

}

void UIRegelaars::NewSnelheidSlot(int regelaarNummer, int snelheid)
{
    mRegelaars[regelaarNummer].snelheid = snelheid;
    updateRegelaar(regelaarNummer);
}

void UIRegelaars::RichtingVeranderingSlot(int regelaarNummer, int richting)
{
    mRegelaars[regelaarNummer].richting = 1-richting;
    updateRegelaar(regelaarNummer);
}

void UIRegelaars::IgnoreStopSlot(int regelaarNummer, bool ignore)
{

}

void UIRegelaars::ShowRegelaarSlot(int regelaarNummer, bool show)
{
    //std::cout << "ShowRegelaarSlot "<< regelaarNummer << " " << show << std::endl;
    if (show)
    {
        mActiveRegelaar = regelaarNummer;
        updateRegelaar(regelaarNummer);
        mRegelaar->show();
    }

}

void UIRegelaars::updateRegelaar(int regelaarNummer)
{
    if (regelaarNummer == mActiveRegelaar)
    {
        QIcon ButtonIcon(*mRegelaars[regelaarNummer].bitmap);
        mRegelaar->ui->bitmap->setIcon(ButtonIcon);
        mRegelaar->ui->snelheid->setValue(mRegelaars[regelaarNummer].snelheid);
        mRegelaar->ui->snelheidProgress->setValue(mRegelaars[regelaarNummer].snelheidProgess);
        mRegelaar->ui->richting->setValue(mRegelaars[regelaarNummer].richting);
    }
}

void UIRegelaars::snelheidSlot(int value)
{
    mRegelaars[mActiveRegelaar].snelheid = value;
    mBaanLib->SetSnelheid(mActiveRegelaar,value);
}

void UIRegelaars::vtValueSlot(int value)
{
    mRegelaars[mActiveRegelaar].richting = value;
    mBaanLib->SetRichting(mActiveRegelaar, 1-value);
}

void UIRegelaars::verwijderClicked()
{
    mBaanLib->VerwijderClicked(mActiveRegelaar);
    mRegelaar->hide();
}
