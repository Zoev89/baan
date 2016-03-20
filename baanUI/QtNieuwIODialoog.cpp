#include <sstream>
#include <iomanip>
#include "QtNieuwIODialoog.h"
#include "ui_QtNieuwIODialoog.h"

QtNieuwIODialoog::QtNieuwIODialoog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtNieuwIODialoog)
    , m_adresValidator(new QDoubleValidator(1, 1023, 2, this))
    , m_blokValidator(new QIntValidator(1, 1023, this))
    , m_adres(0)
    , m_kopBlok(1)
    , m_typeIndex(0)
{
    ui->setupUi(this);
    ui->kopBlok->setValidator(m_blokValidator);
    ui->adres->setValidator(m_adresValidator);
}

QtNieuwIODialoog::~QtNieuwIODialoog()
{
    delete ui;
}

bool QtNieuwIODialoog::RunDialogOk()
{
    ui->ioType->setCurrentIndex(m_typeIndex);
    ui->kopBlok->setText(std::to_string(m_kopBlok).c_str());

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << m_adres;
    ui->adres->setText(out.str().c_str());

    auto dialogRet = exec();
    if (dialogRet==QDialog::Accepted)
    {
        m_adres = std::stof(ui->adres->text().toStdString());
        m_kopBlok = std::stoi(ui->kopBlok->text().toStdString());
        m_typeIndex = ui->ioType->currentIndex();
    }
    return dialogRet;

}

void QtNieuwIODialoog::SetAdres(float adres)
{
    m_adres = adres;
}

float QtNieuwIODialoog::GetAdres()
{
    return m_adres;
}

void QtNieuwIODialoog::SetBlok(int blok)
{
    m_kopBlok = blok;
}

int QtNieuwIODialoog::GetBlok()
{
    return m_kopBlok;
}

void QtNieuwIODialoog::SetTypeIndex(int index)
{
    m_typeIndex = index;
}

int QtNieuwIODialoog::GetTypeIndex()
{
    return m_typeIndex;
}

void QtNieuwIODialoog::SetTypeLabels(std::vector<std::string> labels)
{
    ui->ioType->clear();
    for(auto i:labels)
    {
        ui->ioType->addItem(i.c_str());
    }
}
