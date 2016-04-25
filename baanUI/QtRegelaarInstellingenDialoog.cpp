#include "QtRegelaarInstellingenDialoog.h"
#include "ui_QtRegelaarInstellingenDialoog.h"
#include <iostream>

QtRegelaarInstellingenDialoog::QtRegelaarInstellingenDialoog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtRegelaarInstellingenDialoog)
{
    ui->setupUi(this);
}

QtRegelaarInstellingenDialoog::~QtRegelaarInstellingenDialoog()
{
    delete ui;
}


bool QtRegelaarInstellingenDialoog::RunDialogOk()
{
    auto dialogRet = exec();
    if (dialogRet==QDialog::Accepted)
    {
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
    m_alpha = alpha;
}

float QtRegelaarInstellingenDialoog::GetAlphaRijden()
{
    return m_alpha;
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
