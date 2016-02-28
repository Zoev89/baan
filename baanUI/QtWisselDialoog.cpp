#include "QtWisselDialoog.h"
#include "ui_QtWisselDialoog.h"
#include <sstream>
#include <iomanip>

QtWisselDialoog::QtWisselDialoog(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::QtWisselDialoog)
  , m_adresValidator(new QDoubleValidator(1, 1023, 2, this))
  , m_intValidator(new QIntValidator(-99999, 99999, this))

{
    ui->setupUi(this);
    for(int i=0; i<editFields; ++i)
    {
        m_editAansluiting[i] = new QLineEdit(ui->gridLayoutWidget);
        ui->inputMatrix->addWidget(m_editAansluiting[i], 1, i+2);

        m_editLengte[i] = new QLineEdit(ui->gridLayoutWidget);
        ui->inputMatrix->addWidget(m_editLengte[i], 2, i+2);

        m_editMaxSnelheid[i] = new QLineEdit(ui->gridLayoutWidget);
        ui->inputMatrix->addWidget(m_editMaxSnelheid[i], 3, i+2);
    }
    ui->adres->setValidator(m_adresValidator);
    for(int i=0; i<editFields; ++i)
    {
        m_editLengte[i]->setValidator(m_intValidator);
        m_editMaxSnelheid[i]->setValidator(m_intValidator);
    }
    ui->hwAan->setValidator(m_intValidator);
    ui->hwTot->setValidator(m_intValidator);
    ui->aanTijd->setValidator(m_intValidator);
    ui->uitTijd->setValidator(m_intValidator);
    ui->hoogte->setValidator(m_intValidator);
    ui->breedte->setValidator(m_intValidator);

    ClearAllFields();
}

QtWisselDialoog::~QtWisselDialoog()
{
    delete ui;
}

void QtWisselDialoog::ClearAllFields()
{
    m_uitleg.clear();
    m_adres = 0;
    m_hwAan = boost::none;
    m_hwAanToolTip.clear();
    m_hwTot = 0;
    m_hwTotToolTip.clear();
    m_typeIndex = 0;
    m_typeLabels.clear();
    m_aanTijd = 0;
    m_aanTijdToolTip.clear();
    m_uitTijd = 0;
    m_uitTijdToolTip.clear();
    m_hoogte = 0;
    m_hoogteToolTip.clear();
    m_breedte = 0;
    m_breedteToolTip.clear();
    ui->type->clear();
    for(int i=0; i<editFields; ++i)
    {
        m_aansluiting[i] = boost::none;
        m_aansluitingToolTip[i] = boost::none;
        m_lengte[i] = boost::none;
        m_lengteToolTip[i] = boost::none;
        m_maxSnelheid[i] = boost::none;
        m_maxSnelheidToolTip[i] = boost::none;
    }

}

bool QtWisselDialoog::RunDialogOk()
{
    ui->uitleg->setText(m_uitleg.c_str());

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << m_adres;
    ui->adres->setText(out.str().c_str());
    if (m_hwAan)
    {
        // de settings voor een general IO
        ui->stack->setCurrentIndex(1);

        ui->hwAan->setText(std::to_string(m_hwAan.get()).c_str());
        ui->hwAan->setToolTip(m_hwAanToolTip.c_str());
        ui->hwTot->setText(std::to_string(m_hwTot).c_str());
        ui->hwTot->setToolTip(m_hwTotToolTip.c_str());

        for(auto& i: m_typeLabels)
            ui->type->addItem(i.c_str());
        ui->type->setCurrentIndex(m_typeIndex);

        ui->aanTijd->setText(std::to_string(m_aanTijd).c_str());
        ui->aanTijd->setToolTip(m_aanTijdToolTip.c_str());
        ui->uitTijd->setText(std::to_string(m_uitTijd).c_str());
        ui->uitTijd->setToolTip(m_uitTijdToolTip.c_str());

        ui->hoogte->setText(std::to_string(m_hoogte).c_str());
        ui->hoogte->setToolTip(m_hoogteToolTip.c_str());
        ui->breedte->setText(std::to_string(m_breedte).c_str());
        ui->breedte->setToolTip(m_breedteToolTip.c_str());

    }
    else
    {
        // de settings voor wissels
        ui->stack->setCurrentIndex(0);
        for(int i=0; i<editFields; ++i)
        {
            m_editAansluiting[i]->setEnabled(m_aansluiting[i]);
            m_editAansluiting[i]->setText((m_aansluiting[i]) ? m_aansluiting[i].get().c_str():"");
            m_editAansluiting[i]->setToolTip((m_aansluitingToolTip[i]) ? m_aansluitingToolTip[i].get().c_str():"");

            m_editLengte[i]->setEnabled(m_lengte[i]);
            m_editLengte[i]->setText((m_lengte[i]) ? std::to_string(m_lengte[i].get()).c_str():"");
            m_editLengte[i]->setToolTip((m_lengteToolTip[i]) ? m_lengteToolTip[i].get().c_str():"");

            m_editMaxSnelheid[i]->setEnabled(m_maxSnelheid[i]);
            m_editMaxSnelheid[i]->setText((m_maxSnelheid[i]) ? std::to_string(m_maxSnelheid[i].get()).c_str():"");
            m_editMaxSnelheid[i]->setToolTip((m_maxSnelheidToolTip[i]) ? m_maxSnelheidToolTip[i].get().c_str():"");
        }
    }

    auto dialogRet = exec();
    if (dialogRet==QDialog::Accepted)
    {

        m_adres = std::stof(ui->adres->text().toStdString());
        if (m_hwAan)
        {
            m_hwAan = std::stoi(ui->hwAan->text().toStdString());
            m_hwTot = std::stoi(ui->hwTot->text().toStdString());

            m_typeIndex = ui->type->currentIndex();

            m_aanTijd = std::stoi(ui->aanTijd->text().toStdString());
            m_uitTijd = std::stoi(ui->uitTijd->text().toStdString());

            m_hoogte = std::stoi(ui->hoogte->text().toStdString());
            m_breedte = std::stoi(ui->breedte->text().toStdString());
        }
        else
        {
            for(int i=0; i<editFields; ++i)
            {
                if (m_aansluiting[i])
                {
                    m_aansluiting[i] = m_editAansluiting[i]->text().toStdString();
                }
                if (m_lengte[i])
                {
                    m_lengte[i] = std::stoi(m_editLengte[i]->text().toStdString());
                }
                if (m_maxSnelheid[i])
                {
                    m_maxSnelheid[i] = std::stoi(m_editMaxSnelheid[i]->text().toStdString());
                }
            }

        }


        return true;
    }
    return false;
}

void QtWisselDialoog::SetAdres(float adres)
{
    m_adres = adres;

}

float QtWisselDialoog::GetAdres()
{
    return m_adres;
}


void QtWisselDialoog::SetUitleg(const std::string &uitleg)
{
    m_uitleg = uitleg;
}

std::string QtWisselDialoog::GetUitleg()
{
    return m_uitleg;
}

void QtWisselDialoog::SetHwAan(int hwAan)
{
    m_hwAan = hwAan;
}

int QtWisselDialoog::GetHwAan()
{
    return m_hwAan.get();

}

void QtWisselDialoog::SetHwAanToolTip(const std::string &tooltip)
{
    m_hwAanToolTip = tooltip;

}


void QtWisselDialoog::SetHwTot(int hwTot)
{
    m_hwTot = hwTot;
}

int QtWisselDialoog::GetHwTot()
{
    return m_hwTot;
}

void QtWisselDialoog::SetHwTotToolTip(const std::string &tooltip)
{
    m_hwTotToolTip = tooltip;
}


void QtWisselDialoog::SetTypeIndex(int index)
{
    m_typeIndex = index;
}

int QtWisselDialoog::GetTypeIndex()
{
    return m_typeIndex;
}

void QtWisselDialoog::SetTypeLabels(std::vector<std::string> labels)
{
    m_typeLabels = labels;
}

std::vector<std::string> QtWisselDialoog::GetTypeLabels()
{
    return m_typeLabels;
}

void QtWisselDialoog::SetAanTijd(int ticks)
{
    m_aanTijd = ticks;
}

int QtWisselDialoog::GetAanTijd()
{
    return m_aanTijd;
}

void QtWisselDialoog::SetAanTijdToolTip(const std::string &tooltip)
{
    m_aanTijdToolTip = tooltip;
}


void QtWisselDialoog::SetUitTijd(int ticks)
{
    m_uitTijd = ticks;
}

int QtWisselDialoog::GetUitTijd()
{
    return m_uitTijd;
}

void QtWisselDialoog::SetUitTijdToolTip(const std::string &tooltip)
{
    m_uitTijdToolTip = tooltip;
}


void QtWisselDialoog::SetHoogte(int hoogte)
{
    m_hoogte = hoogte;
}

int QtWisselDialoog::GetHoogte()
{
    return m_hoogte;
}

void QtWisselDialoog::SetHoogteToolTip(const std::string &tooltip)
{
    m_hoogteToolTip = tooltip;
}


void QtWisselDialoog::SetBreedte(int breedte)
{
    m_breedte = breedte;
}

int QtWisselDialoog::GetBreedte()
{
    return m_breedte;
}

void QtWisselDialoog::SetBreedteToolTip(const std::string &tooltip)
{
    m_breedteToolTip = tooltip;
}


void QtWisselDialoog::SetAansluiting(int index, const std::string &aansluiting)
{
    m_aansluiting[index] = aansluiting;
}

std::string QtWisselDialoog::GetAansluiting(int index)
{
    return m_aansluiting[index].get();
}

void QtWisselDialoog::SetAansluitingToolTip(int index, const std::string &tooltip)
{
    m_aansluitingToolTip[index] = tooltip;
}

void QtWisselDialoog::SetLengte(int index, int lengte)
{
    m_lengte[index] = lengte;
}

int QtWisselDialoog::GetLengte(int index)
{
    return m_lengte[index].get();
}


void QtWisselDialoog::SetLengteToolTip(int index, const std::string &tooltip)
{
    m_lengteToolTip[index] = tooltip;

}

void QtWisselDialoog::SetMaxSnelheid(int index,int snelheid)
{
    m_maxSnelheid[index] = snelheid;
}

int QtWisselDialoog::GetMaxSnelheid(int index)
{
    return m_maxSnelheid[index].get();
}

void QtWisselDialoog::SetMaxSnelheidToolTip(int index, const std::string &tooltip)
{
    m_maxSnelheidToolTip[index] = tooltip;
}

