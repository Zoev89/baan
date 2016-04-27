#include "QtRegelaarInstellingenDialoog.h"
#include "ui_QtRegelaarInstellingenDialoog.h"
#include <iostream>
#include <sstream>


class DoubleValidator : public QDoubleValidator
{
public:
    DoubleValidator(double bottom, double top, int decimals, QObject * parent) :
        QDoubleValidator(bottom, top, decimals, parent)
    {
    }

    // limit checking is niet goed in QDoubleValidator want die returned Intermediate
    // en QlineEdit heeft Invalid nodig dan
    QValidator::State validate(QString &s, int &i) const
    {
        auto ret = QDoubleValidator::validate(s,i);
        if (ret == QValidator::Intermediate)
            return QValidator::Invalid;
        return ret;
    }
};

QtRegelaarInstellingenDialoog::QtRegelaarInstellingenDialoog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtRegelaarInstellingenDialoog)
    , m_snelheidValidator(new QIntValidator(3, 60, this))
    , m_posValidator(new QIntValidator(0, 999999, this))
    , m_hhmmRx("^[0-9]{1,6}:[0-5][0-9]")
    , m_tijdhhmmValidator(new QRegExpValidator(m_hhmmRx, this))
    , m_float0_1Validator(new DoubleValidator(0, 1, 4, this))
    , m_floatPosValidator(new DoubleValidator(0, 10000, 2, this))

{
    ui->setupUi(this);
    ui->minSnelheid->setValidator(m_snelheidValidator);
    ui->maxSnelheid->setValidator(m_snelheidValidator);
    ui->topSnelheid->setValidator(m_posValidator);
    ui->totaalAfstand->setValidator(m_posValidator);
    ui->totaalTijd->setValidator(m_tijdhhmmValidator);
    ui->lengte->setValidator(m_posValidator);
    ui->clipRijden->setValidator(m_posValidator);
    ui->clipStoppen->setValidator(m_posValidator);
    ui->alphaRijden->setValidator(m_float0_1Validator);
    ui->alphaStoppen->setValidator(m_float0_1Validator);
    ui->stand1->setValidator(m_snelheidValidator);
    ui->stand2->setValidator(m_snelheidValidator);
    ui->afstand1->setValidator(m_posValidator);
    ui->afstand2->setValidator(m_posValidator);
    ui->kLpf->setValidator(m_floatPosValidator);
    ui->plusMinus->setValidator(m_floatPosValidator);
    ui->helling->setValidator(m_floatPosValidator);
    ui->dodeTijd->setValidator(m_floatPosValidator);
    ui->langzaam->setValidator(m_snelheidValidator);
    ui->rijden->setValidator(m_snelheidValidator);
    ui->lastStand1->setValidator(m_snelheidValidator);
    ui->lastStand2->setValidator(m_snelheidValidator);
    ui->lastGain1->setValidator(m_floatPosValidator);
    ui->lastGain2->setValidator(m_floatPosValidator);
    ui->lastRegelingKeuze->setValidator(m_posValidator);
}

QtRegelaarInstellingenDialoog::~QtRegelaarInstellingenDialoog()
{
    delete ui;
}

std::string QtRegelaarInstellingenDialoog::toString(float value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}

bool QtRegelaarInstellingenDialoog::RunDialogOk()
{
    ui->laatsteWagonCheck->setChecked(m_laatsteWagonCheck);
    ui->eloc->setChecked(m_eloc);
    ui->minSnelheid->setText(std::to_string(m_minSnelheid).c_str());
    ui->maxSnelheid->setText(std::to_string(m_maxSnelheid).c_str());
    ui->topSnelheid->setText(std::to_string(m_topSnelheid).c_str());
    ui->locType->setText(m_locType.c_str());
    ui->totaalAfstand->setText(std::to_string(m_totaalAfstand).c_str());
    ui->totaalTijd->setText(m_totaalTijd.c_str());
    ui->lengte->setText(std::to_string(m_lengte).c_str());
    ui->errors->setText(m_errors.c_str());
    ui->alphaRijden->setText(toString(m_alphaRijden).c_str());
    ui->alphaStoppen->setText(toString(m_alphaStoppen).c_str());
    ui->clipRijden->setText(std::to_string(m_clipRijden).c_str());
    ui->clipStoppen->setText(std::to_string(m_clipStoppen).c_str());
    ui->stand1->setText(std::to_string(m_stand1).c_str());
    ui->stand2->setText(std::to_string(m_stand2).c_str());
    ui->afstand1->setText(std::to_string(m_afstand1).c_str());
    ui->afstand2->setText(std::to_string(m_afstand2).c_str());
    ui->kLpf->setText(toString(m_kLpf).c_str());
    ui->plusMinus->setText(toString(m_plusMinus).c_str());
    ui->helling->setText(toString(m_helling).c_str());
    ui->dodeTijd->setText(toString(m_dodeTijd).c_str());
    ui->programmaNaam->setText(m_programmaNaam.c_str());
    ui->herlaadProgramma->setChecked(m_laatsteWagonCheck);
    ui->langzaam->setText(std::to_string(m_langzaam).c_str());
    ui->rijden->setText(std::to_string(m_rijden).c_str());
    ui->lastStand1->setText(std::to_string(m_lastStand1).c_str());
    ui->lastStand2->setText(std::to_string(m_lastStand2).c_str());
    ui->lastRegelingKeuze->setText(std::to_string(m_lastKeuze).c_str());
    ui->lastGain1->setText(toString(m_lastGain1).c_str());
    ui->lastGain2->setText(toString(m_lastGain2).c_str());

    auto dialogRet = exec();
    if (dialogRet==QDialog::Accepted)
    {
        m_laatsteWagonCheck = ui->laatsteWagonCheck->isChecked();
        m_eloc = ui->eloc->isChecked();
        m_minSnelheid = std::stoi(ui->minSnelheid->text().toStdString());
        m_maxSnelheid = std::stoi(ui->maxSnelheid->text().toStdString());
        m_topSnelheid = std::stoi(ui->topSnelheid->text().toStdString());
        m_locType = ui->locType->text().toStdString();
        m_totaalAfstand = std::stoi(ui->totaalAfstand->text().toStdString());
        m_totaalTijd = ui->totaalTijd->text().toStdString();
        m_lengte = std::stoi(ui->lengte->text().toStdString());
        m_alphaRijden = std::stof(ui->alphaRijden->text().toStdString());
        m_alphaStoppen = std::stof(ui->alphaStoppen->text().toStdString());
        m_clipRijden = std::stoi(ui->clipRijden->text().toStdString());
        m_clipStoppen = std::stoi(ui->clipStoppen->text().toStdString());
        m_stand1 = std::stoi(ui->stand1->text().toStdString());
        m_stand2 = std::stoi(ui->stand2->text().toStdString());
        m_afstand1 = std::stoi(ui->afstand1->text().toStdString());
        m_afstand2 = std::stoi(ui->afstand2->text().toStdString());
        m_kLpf = std::stof(ui->kLpf->text().toStdString());
        m_plusMinus = std::stof(ui->plusMinus->text().toStdString());
        m_helling = std::stof(ui->helling->text().toStdString());
        m_dodeTijd = std::stof(ui->dodeTijd->text().toStdString());
        m_programmaNaam = ui->programmaNaam->text().toStdString();
        m_herlaadProgramma = ui->herlaadProgramma->isChecked();
        m_langzaam = std::stoi(ui->langzaam->text().toStdString());
        m_rijden = std::stoi(ui->rijden->text().toStdString());
        m_lastStand1 = std::stoi(ui->lastStand1->text().toStdString());
        m_lastGain1 = std::stof(ui->lastGain1->text().toStdString());
        m_lastStand2 = std::stoi(ui->lastStand2->text().toStdString());
        m_lastGain2 = std::stof(ui->lastGain2->text().toStdString());
        m_lastKeuze = std::stoi(ui->lastRegelingKeuze->text().toStdString());
        return true;
    }
    return false;

}

void QtRegelaarInstellingenDialoog::SetLaatsteWagonCheck(bool laatsteWagon)
{
    m_laatsteWagonCheck = laatsteWagon;
}

bool QtRegelaarInstellingenDialoog::GetLaatsteWagonCheck()
{
    return m_laatsteWagonCheck;
}

void QtRegelaarInstellingenDialoog::SetEloc(bool eloc)
{
    m_eloc = eloc;
}

bool QtRegelaarInstellingenDialoog::GetEloc()
{
    return m_eloc;
}

void QtRegelaarInstellingenDialoog::SetMinSnelheid(int snelheid)
{
    m_minSnelheid = snelheid;
}

int QtRegelaarInstellingenDialoog::GetMinSnelheid()
{
    return m_minSnelheid;
}

void QtRegelaarInstellingenDialoog::SetMaxSnelheid(int snelheid)
{
    m_maxSnelheid = snelheid;
}

int QtRegelaarInstellingenDialoog::GetMaxSnelheid()
{
    return m_maxSnelheid;
}

void QtRegelaarInstellingenDialoog::SetTopSnelheid(int snelheid)
{
    m_topSnelheid = snelheid;
}

int QtRegelaarInstellingenDialoog::GetTopSnelheid()
{
    return m_topSnelheid;
}


void QtRegelaarInstellingenDialoog::SetTotaalAfstand(int afstand)
{
    m_totaalAfstand = afstand;
}

int QtRegelaarInstellingenDialoog::GetTotaalAfstand()
{
    return m_totaalAfstand;
}

void QtRegelaarInstellingenDialoog::SetTotaalTijd(const std::string &tijd)
{
    m_totaalTijd = tijd;
}

std::string QtRegelaarInstellingenDialoog::GetTotaalTijd()
{
    return m_totaalTijd;
}

void QtRegelaarInstellingenDialoog::SetLengte(int lengte)
{
    m_lengte = lengte;
}

int QtRegelaarInstellingenDialoog::GetLengte()
{
    return m_lengte;
}

void QtRegelaarInstellingenDialoog::SetAlphaRijden(float alpha)
{
    m_alphaRijden = alpha;
}

float QtRegelaarInstellingenDialoog::GetAlphaRijden()
{
    return m_alphaRijden;
}

void QtRegelaarInstellingenDialoog::SetClipRijden(int clip)
{
    m_clipRijden = clip;
}

int QtRegelaarInstellingenDialoog::GetClipRijden()
{
    return m_clipRijden;
}

void QtRegelaarInstellingenDialoog::SetAlphaStoppen(float alpha)
{
    m_alphaStoppen = alpha;
}

float QtRegelaarInstellingenDialoog::GetAlphaStoppen()
{
    return m_alphaStoppen;
}

void QtRegelaarInstellingenDialoog::SetClipStoppen(int clip)
{
    m_clipStoppen = clip;
}

int QtRegelaarInstellingenDialoog::GetClipStoppen()
{
    return m_clipStoppen;
}

void QtRegelaarInstellingenDialoog::SetStand1(int snelheid)
{
    m_stand1 = snelheid;
}

int QtRegelaarInstellingenDialoog::GetStand1()
{
    return m_stand1;
}

void QtRegelaarInstellingenDialoog::SetAfstand1(int afstand)
{
    m_afstand1 = afstand;
}

int QtRegelaarInstellingenDialoog::GetAfstand1()
{
    return m_afstand1;
}

void QtRegelaarInstellingenDialoog::SetStand2(int snelheid)
{
    m_stand2 = snelheid;
}

int QtRegelaarInstellingenDialoog::GetStand2()
{
    return m_stand2;
}

void QtRegelaarInstellingenDialoog::SetAfstand2(int afstand)
{
    m_afstand2 = afstand;
}

int QtRegelaarInstellingenDialoog::GetAfstand2()
{
    return m_afstand2;
}


void QtRegelaarInstellingenDialoog::SetErrors(const std::string &errorString)
{
    m_errors = errorString;
}

std::string QtRegelaarInstellingenDialoog::GetErrors()
{
    return m_errors;
}

void QtRegelaarInstellingenDialoog::SetKLpf(float k)
{
    m_kLpf = k;
}

float QtRegelaarInstellingenDialoog::GetKLpf()
{
    return m_kLpf;
}

void QtRegelaarInstellingenDialoog::SetPlusMinus(float plusMinus)
{
    m_plusMinus = plusMinus;
}

float QtRegelaarInstellingenDialoog::GetPlusMinus()
{
    return m_plusMinus;
}

void QtRegelaarInstellingenDialoog::SetHelling(float helling)
{
    m_helling = helling;
}

float QtRegelaarInstellingenDialoog::GetHelling()
{
    return m_helling;
}

void QtRegelaarInstellingenDialoog::SetDodeTijd(float dodeTijd)
{
    m_dodeTijd = dodeTijd;
}

float QtRegelaarInstellingenDialoog::GetDodeTijd()
{
    return m_dodeTijd;
}

void QtRegelaarInstellingenDialoog::SetProgrammaNaam(const std::string &naam)
{
    m_programmaNaam = naam;
}

std::string QtRegelaarInstellingenDialoog::GetProgrammaNaam()
{
    return m_programmaNaam;
}

void QtRegelaarInstellingenDialoog::SetHerlaadProgramma(bool herlaad)
{
    m_herlaadProgramma = herlaad;
}

bool QtRegelaarInstellingenDialoog::GetHerlaadProgramma()
{
    return m_herlaadProgramma;
}

void QtRegelaarInstellingenDialoog::SetLangzaam(int snelheid)
{
    m_langzaam = snelheid;
}

int QtRegelaarInstellingenDialoog::GetLangzaam()
{
    return m_langzaam;
}

void QtRegelaarInstellingenDialoog::SetRijden(int snelheid)
{
    m_rijden = snelheid;
}

int QtRegelaarInstellingenDialoog::GetRijden()
{
    return m_rijden;
}

void QtRegelaarInstellingenDialoog::SetLocType(const std::string &locType)
{
    m_locType = locType;
}

std::string QtRegelaarInstellingenDialoog::GetLocType()
{
    return m_locType;
}

void QtRegelaarInstellingenDialoog::SetLastStand1(int stand)
{
    m_lastStand1 = stand;
}

int QtRegelaarInstellingenDialoog::GetLastStand1()
{
    return m_lastStand1;
}

void QtRegelaarInstellingenDialoog::SetLastGain1(float gain)
{
    m_lastGain1 = gain;
}

float QtRegelaarInstellingenDialoog::GetLastGain1()
{
    return m_lastGain1;
}

void QtRegelaarInstellingenDialoog::SetLastStand2(int stand)
{
    m_lastStand2 = stand;
}

int QtRegelaarInstellingenDialoog::GetLastStand2()
{
    return m_lastStand2;
}

void QtRegelaarInstellingenDialoog::SetLastGain2(float gain)
{
    m_lastGain2 = gain;
}

float QtRegelaarInstellingenDialoog::GetLastGain2()
{
    return m_lastGain2;

}

void QtRegelaarInstellingenDialoog::SetLastRegelKeuze(int keuzeIndex)
{
    m_lastKeuze = keuzeIndex;
}

int QtRegelaarInstellingenDialoog::GetLastRegelKeuze()
{
    return m_lastKeuze;
}
void QtRegelaarInstellingenDialoog::SetStandUitleg(const std::string &uitleg)
{
    ui->stand1->setToolTip(uitleg.c_str());
    ui->stand2->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetAlphaUitleg(const std::string &uitleg)
{
    ui->alphaRijden->setToolTip(uitleg.c_str());
    ui->alphaStoppen->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetClipUitleg(const std::string &uitleg)
{
    ui->clipRijden->setToolTip(uitleg.c_str());
    ui->clipStoppen->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetKlpfUitleg(const std::string &uitleg)
{
    ui->kLpf->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetPlusMinusUitleg(const std::string &uitleg)
{
    ui->plusMinus->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetHellingUitleg(const std::string &uitleg)
{
    ui->helling->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetDodeTijdUitleg(const std::string &uitleg)
{
    ui->dodeTijd->setToolTip(uitleg.c_str());
}

void QtRegelaarInstellingenDialoog::SetLangzaamRijdenUitleg(const std::string &uitleg)
{
    ui->langzaam->setToolTip(uitleg.c_str());
    ui->rijden->setToolTip(uitleg.c_str());
}

