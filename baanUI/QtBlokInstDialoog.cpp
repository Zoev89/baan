#include "QtBlokInstDialoog.h"
#include "ui_QtBlokInstDialoog.h"

QtBlokInstDialoog::QtBlokInstDialoog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtBlokInstDialoog)
    , m_blokValidator(new QIntValidator(1, 1023, this))
    , m_intValidator(new QIntValidator(1, 99999, this))
    , m_maxSnelheidValidator(new QIntValidator(-1, 9999, this))
    , m_blokNummer(1)
    , m_maxSnelheid(0)
    , m_lengte(0)
    , m_bovenleiding(false)
    , m_seinIndex(0)
    , m_voorKeurRichtingIndex(0)
{
    ui->setupUi(this);
    ui->blok->setValidator(m_blokValidator);
    ui->lengte->setValidator(m_intValidator);
    ui->maxSnelheid->setValidator(m_maxSnelheidValidator);
    ui->seinType->addItem("Geen");
    ui->seinType->addItem("Rood Groen");
    ui->seinType->addItem("Rood Groen Geel");
}

QtBlokInstDialoog::~QtBlokInstDialoog()
{
    delete ui;
}


bool QtBlokInstDialoog::RunDialogOk()
{
    ui->blok->setText(std::to_string(m_blokNummer).c_str());
    ui->volgendBlok->setText(m_volgendBlok.c_str());
    ui->lengte->setText(std::to_string(m_lengte).c_str());
    ui->maxSnelheid->setText(std::to_string(m_maxSnelheid).c_str());
    ui->bovenLeiding->setChecked(m_bovenleiding);
    ui->seinType->setCurrentIndex(m_seinIndex);

    auto dialogRet = exec();

    if (dialogRet==QDialog::Accepted)
    {
        m_blokNummer = std::stoi(ui->blok->text().toStdString());
        m_volgendBlok =  ui->volgendBlok->text().toStdString();
        m_bovenleiding = ui->bovenLeiding->isChecked();
        m_maxSnelheid = std::stoi(ui->maxSnelheid->text().toStdString());
        m_lengte = std::stoi(ui->lengte->text().toStdString());
        m_seinIndex = ui->seinType->currentIndex();

        return true;
    }
    return false;
}

void QtBlokInstDialoog::SetBlok(int blokNummer)
{
    m_blokNummer = blokNummer;
}

int QtBlokInstDialoog::GetBlok()
{
    return m_blokNummer;
}

void QtBlokInstDialoog::SetVolgendBlok(const std::string &blok)
{
    m_volgendBlok = blok;
}

std::string QtBlokInstDialoog::GetVolgendBlok()
{
    return m_volgendBlok;
}

void QtBlokInstDialoog::SetMaxSnelheid(int maxSnelheid)
{
    m_maxSnelheid = maxSnelheid;
}

int QtBlokInstDialoog::GetMaxSnelheid()
{
    return m_maxSnelheid;
}

void QtBlokInstDialoog::SetLengte(int lengte)
{
    m_lengte = lengte;
}

int QtBlokInstDialoog::GetLengte()
{
    return m_lengte;
}

void QtBlokInstDialoog::SetBovenLeiding(bool bovenleiding)
{
    m_bovenleiding = bovenleiding;
}

bool QtBlokInstDialoog::GetBovenLeiding()
{
    return m_bovenleiding;
}

void QtBlokInstDialoog::SetSein(int seinIndex)
{
    m_seinIndex = seinIndex;
}

int QtBlokInstDialoog::GetSein()
{
    return m_seinIndex;
}

void QtBlokInstDialoog::SetVoorKeurRichting(int voorKeurRichtingIndex)
{
    m_voorKeurRichtingIndex = voorKeurRichtingIndex;
}

int QtBlokInstDialoog::GetVoorKeurRichting()
{
    return m_voorKeurRichtingIndex;
}

