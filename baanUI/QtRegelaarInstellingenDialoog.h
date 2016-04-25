#ifndef QTREGELAARINSTELLINGENDIALOOG_H
#define QTREGELAARINSTELLINGENDIALOOG_H
#include "IRegelaarInstellingenDialoog.h"

#include <QDialog>

namespace Ui {
class QtRegelaarInstellingenDialoog;
}

class QtRegelaarInstellingenDialoog : public QDialog , public IRegelaarInstellingenDialoog
{
    Q_OBJECT

public:
    explicit QtRegelaarInstellingenDialoog(QWidget *parent = 0);
    ~QtRegelaarInstellingenDialoog();

    virtual bool RunDialogOk() override; // returns true when ok is pressed

    virtual void  SetLaatsteWagonCheck(bool laatsteWagon) override;
    virtual bool  GetLaatsteWagonCheck() override;
    virtual void  SetEloc(bool eloc) override;
    virtual bool  GetEloc() override;
    virtual void  SetMinSnelheid(int snelheid) override;
    virtual int   GetMinSnelheid() override;
    virtual void  SetMaxSnelheid(int snelheid) override;
    virtual int   GetMaxSnelheid() override;
    virtual void  SetTopSnelheid(int snelheid) override;
    virtual int   GetTopSnelheid() override;
    virtual void  SetTotaalAfstand(int afstand) override; // in m
    virtual int   GetTotaalAfstand() override;
    virtual void  SetTotaalTijd(const std::string& tijd) override; // hh:mm
    virtual std::string GetTotaalTijd() override;
    virtual void  SetLengte(int lengte) override; // in cm
    virtual int   GetLengte() override;
    virtual void  SetAlphaRijden(float alpha) override;
    virtual float GetAlphaRijden() override;
    virtual void  SetClipRijden(int clip) override;
    virtual int   GetClipRijden() override;
    virtual void  SetAlphaStoppen(float alpha) override;
    virtual float GetAlphaStoppen() override;
    virtual void  SetClipStoppen(int clip) override;
    virtual int   GetClipStoppen() override;
    virtual void  SetStand1(int snelheid) override;
    virtual int   GetStand1() override;
    virtual void  SetAfstand1(int afstand) override; // in cm
    virtual int   GetAfstand1() override;
    virtual void  SetStand2(int snelheid) override;
    virtual int   GetStand2() override;
    virtual void  SetAfstand2(int afstand) override; // in cm
    virtual int   GetAfstand2() override;
    virtual void  SetErrors(const std::string& errorString) override;
    virtual std::string GetErrors() override;
    virtual void  SetKLpf(float k) override;
    virtual float GetKLpf() override;
    virtual void  SetPlusMinus(float plusMinus) override;
    virtual float GetPlusMinus() override;
    virtual void  SetHelling(float helling) override;
    virtual float GetHelling() override;
    virtual void  SetDodeTijd(float dodeTijd) override;
    virtual float GetDodeTijd() override;
    virtual void  SetProgrammaNaam(const std::string& naam) override;
    virtual std::string GetProgrammaNaam() override;
    virtual void  SetHerlaadProgramma(bool herlaad) override;
    virtual bool  GetHerlaadProgramma() override;
    virtual void  SetLangzaam(int snelheid) override;
    virtual int   GetLangzaam() override;
    virtual void  SetRijden(int snelheid) override;
    virtual int   GetRijden() override;
    virtual void  SetLocType(const std::string& locType) override;
    virtual std::string GetLocType() override;
    virtual void  SetLastStand1(int stand) override;
    virtual int   GetLastStand1() override;
    virtual void  SetLastGain1(float gain) override;
    virtual float GetLastGain1() override;
    virtual void  SetLastStand2(int stand) override;
    virtual int   GetLastStand2() override;
    virtual void  SetLastGain2(float gain) override;
    virtual float GetLastGain2() override;
    virtual void  SetLastRegelKeuze(int keuzeIndex) override;
    virtual int   GetLastRegelKeuze() override;

private:
    Ui::QtRegelaarInstellingenDialoog *ui;
    bool m_laatsteWagonCheck;
    bool m_eloc;
    int m_minSnelheid;
    int m_maxSnelheid;
    int m_topSnelheid;
    int m_totaalAfstand;
    std::string m_totaalTijd;
    int m_lengte;
    float m_alpha;
    int m_clipRijden;
    float m_alphaStoppen;
    int m_clipStoppen;
    int m_stand1;
    int m_afstand1;
    int m_stand2;
    int m_afstand2;
    std::string m_errors;
    float m_kLpf;
    float m_plusMinus;
    float m_helling;
    float m_dodeTijd;
    std::string m_programmaNaam;
    bool m_herlaadProgramma;
    int m_langzaam;
    int m_rijden;
    std::string m_locType;
    int m_lastStand1;
    float m_lastGain1;
    int m_lastStand2;
    float m_lastGain2;
    int m_lastKeuze;
};

#endif // QTREGELAARINSTELLINGENDIALOOG_H
