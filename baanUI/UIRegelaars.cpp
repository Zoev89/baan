#include "UIRegelaars.h"
#include <QLabel>

UIRegelaars::UIRegelaars(QWidget *parent, BaanLib *baanLib)
    : QDialog(parent)
    , mBaanLib(baanLib)
    , mActiveRegelaar(0)
    , ui(new Ui::QtTreinenDialoog)
{
    mRegelaar = new UIRegelaar(this);
    ui->setupUi(this);

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treinen);
    treeItem->setText(0, "aap");
    ui->treinen->addTopLevelItem(treeItem);
    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(0, "aapje");
    treeItem->addChild(childItem);

    treeItem = new QTreeWidgetItem(ui->treinen);
    treeItem->setText(0, "note");
    ui->treinen->addTopLevelItem(treeItem);

}

QPixmap* UIRegelaars::GetBitmap(int regelaarNummer)
{
    return mRegelaars[regelaarNummer].bitmap;
}


void UIRegelaars::NieuweRegelaarSlot(int regelaarNummer, const QString bitmapFileName)
{
    //std::cout << "reg " << regelaarNummer << "  " << bitmapFileName.toStdString() << std::endl;

    mRegelaars[regelaarNummer].bitmap = new QPixmap(bitmapFileName);

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treinen);
    //treeItem->setFont(0,  QFont("Times", 15, QFont::Bold));
    treeItem->setText(2, "wat");
    auto label = new QLabel(ui->treinen);
    label->setPixmap(*mRegelaars[regelaarNummer].bitmap);
    ui->treinen->addTopLevelItem(treeItem);
    ui->treinen->setItemWidget(treeItem, 0, label);
    auto progress = new QProgressBar(ui->treinen);
    progress->setOrientation(Qt::Vertical);
    progress->setMaximum(63);

    ui->treinen->setItemWidget(treeItem, 1 , progress);

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
    mRegelaars[regelaarNummer].ignoreStop = ignore;
    updateRegelaar(regelaarNummer);
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
        show();  //evenhier maar moeten we anders doen

        QIcon ButtonIcon(*mRegelaars[regelaarNummer].bitmap);
        mRegelaar->ui->bitmap->setIcon(ButtonIcon);
        mRegelaar->ui->snelheid->setValue(mRegelaars[regelaarNummer].snelheid);
        mRegelaar->ui->snelheidProgress->setValue(mRegelaars[regelaarNummer].snelheidProgess);
        mRegelaar->ui->richting->setValue(mRegelaars[regelaarNummer].richting);
        mRegelaar->ui->ignoreStop->setChecked(mRegelaars[regelaarNummer].ignoreStop);
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

void UIRegelaars::ignoreStopChangedSlot(bool ignore)
{
    mRegelaars[mActiveRegelaar].ignoreStop = ignore;
    mBaanLib->SetIgnoreStop(mActiveRegelaar,ignore);
}

void UIRegelaars::instellingenDialogSlot()
{
    mBaanLib->PropertiesClicked(mActiveRegelaar);
}

