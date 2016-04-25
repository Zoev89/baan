#include "UIRegelaar.h"
#include "ui_UIRegelaar.h"
#include <iostream>

UIRegelaar::UIRegelaar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIRegelaar)
{
    ui->setupUi(this);
    connect(this, SIGNAL(snelheidSignal(int)), parent, SLOT(snelheidSlot(int)));
    connect(this, SIGNAL(vtValueSignal(int)), parent, SLOT(vtValueSlot(int)));
    connect(this, SIGNAL(verwijderClickedSignal()), parent, SLOT(verwijderClicked()));
    connect(this, SIGNAL(ignoreStopSignal(bool )), parent, SLOT(ignoreStopChangedSlot(bool)));
    connect(this, SIGNAL(instellingenDialogSignal()), parent, SLOT(instellingenDialogSlot()));

}

UIRegelaar::~UIRegelaar()
{
    delete ui;
}

void UIRegelaar::vtValue(int value)
{
    emit vtValueSignal(value);
}

void UIRegelaar::snelheid(int value)
{
    emit snelheidSignal(value);
}

void UIRegelaar::on_verwijder_clicked()
{
    emit verwijderClickedSignal();
}

void UIRegelaar::on_ignoreStop_clicked(bool checked)
{
    emit ignoreStopSignal(checked);
}

void UIRegelaar::on_bitmap_clicked()
{
    emit instellingenDialogSignal();
}
