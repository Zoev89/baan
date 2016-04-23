#include "QtAddBlokDialoog.h"
#include "ui_QtAddBlokDialoog.h"

QtAddBlokDialoog::QtAddBlokDialoog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtAddBlokDialoog)
    , m_blokValidator(new QIntValidator(1, 1023, this))

{
    ui->setupUi(this);
    ui->blokNummer->setValidator(m_blokValidator);

}

QtAddBlokDialoog::~QtAddBlokDialoog()
{
    delete ui;
}

bool QtAddBlokDialoog::RunDialogOk()
{
    ui->blokNummer->setText(std::to_string(m_blokNummer).c_str());
    auto dialogRet = exec();
    if (dialogRet==QDialog::Accepted)
    {
        m_blokNummer = std::stoi(ui->blokNummer->text().toStdString());
        return true;
    }
    return false;
}

void QtAddBlokDialoog::SetVrijBlok(int blok)
{
    m_blokNummer = blok;
}

int QtAddBlokDialoog::GetVrijBlok()
{
    return m_blokNummer;
}
