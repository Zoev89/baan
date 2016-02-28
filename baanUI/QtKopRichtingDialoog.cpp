#include <QIntValidator>
#include "QtKopRichtingDialoog.h"
#include "ui_QtKopRichtingDialoog.h"

QtKopRichtingDialoog::QtKopRichtingDialoog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtKopRichtingDialoog)
    , m_kopBlokValidator(new QIntValidator(1, 1023, this))
    , m_lengteValidator(new QIntValidator(1, 99999, this))
    , m_kopBlok(0)
    , m_lengte(0)
    , m_kopBlokMee(false)
    , m_vt(false)

{
    ui->setupUi(this);
    ui->KopBlok->setValidator(m_kopBlokValidator);
    ui->Lengte->setValidator(m_lengteValidator);
}

QtKopRichtingDialoog::~QtKopRichtingDialoog()
{
    delete ui;
}

bool QtKopRichtingDialoog::RunDialogOk()
{

    ui->KopBlok->setText(std::to_string(m_kopBlok).c_str());
    ui->errorText->setText(m_errorText.c_str());
    ui->Lengte->setText(std::to_string(m_lengte).c_str());
    ui->NeemKopBlokMee->setChecked(m_kopBlokMee);
    ui->VT->setValue(m_vt);
    auto dialogRet = exec();
    if (dialogRet==QDialog::Accepted)
    {
        m_kopBlok = std::stoi(ui->KopBlok->text().toStdString());
        m_lengte =  std::stoi(ui->Lengte->text().toStdString());
        m_kopBlokMee = ui->NeemKopBlokMee->isChecked();
        m_vt=ui->VT->value();
        return true;
    }
    return false;
}

void QtKopRichtingDialoog::SetKopBlok(int blok)
{
    m_kopBlok = blok;
}

int QtKopRichtingDialoog::GetKopBlok()
{
    return m_kopBlok;
}

void QtKopRichtingDialoog::SetLengte(int lengte)
{
    m_lengte = lengte;
}

int QtKopRichtingDialoog::GetLengte()
{
    return m_lengte;
}

void QtKopRichtingDialoog::SetKopBlokMee(bool check)
{
    m_kopBlokMee = check;
}

bool QtKopRichtingDialoog::GetKopBlokMee()
{
    return m_kopBlokMee;
}

void QtKopRichtingDialoog::SetVooruitTerug(bool sliderVT)
{
    m_vt = sliderVT;
}

bool QtKopRichtingDialoog::GetVoorUitTerug()
{
    return m_vt;
}

void QtKopRichtingDialoog::SetErrorText(const std::string &errorText)
{
    m_errorText = errorText;
}

std::string QtKopRichtingDialoog::GetErrorText()
{
    return m_errorText;
}
