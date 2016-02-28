#include "UIRegelaar.h"
#include "ui_UIRegelaar.h"

UIRegelaar::UIRegelaar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIRegelaar)
{
    ui->setupUi(this);
    connect(this, SIGNAL(snelheidSignal(int)), parent, SLOT(snelheidSlot(int)));
    connect(this, SIGNAL(vtValueSignal(int)), parent, SLOT(vtValueSlot(int)));
    connect(this, SIGNAL(verwijderClickedSignal()), parent, SLOT(verwijderClicked()));

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
