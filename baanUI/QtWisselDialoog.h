#ifndef QTWISSELDIALOOG_H
#define QTWISSELDIALOOG_H
#include "IWisselDialoog.h"
#include <boost/optional.hpp>
#include <QLineEdit>
#include <QDialog>

namespace Ui {
class QtWisselDialoog;
}

class QtWisselDialoog : public QDialog, public IWisselDialoog
{
    Q_OBJECT

public:
    explicit QtWisselDialoog(QWidget *parent = 0);
    ~QtWisselDialoog();

    virtual void ClearAllFields() override; // Omdat de dialoog voor alle wissels word gebruikt moet de context afhankelijk zijn
    virtual bool RunDialogOk() override; // returns true when ok is pressed

    virtual void  SetAdres(float adres) override; // xxx.yy notation
    virtual float GetAdres() override;

    virtual void  SetUitleg(const std::string& uitleg) override;
    virtual std::string GetUitleg() override;

    virtual void  SetHwAan(int hwAan) override;
    virtual int   GetHwAan() override;
    virtual void  SetHwAanToolTip(const std::string& tooltip) override;

    virtual void  SetHwTot(int hwTot) override;
    virtual int   GetHwTot() override;
    virtual void  SetHwTotToolTip(const std::string& tooltip) override;

    virtual void  SetTypeIndex(int index) override;
    virtual int   GetTypeIndex() override;
    virtual void  SetTypeLabels(std::vector<std::string> labels) override;
    virtual std::vector<std::string> GetTypeLabels() override;

    virtual void  SetAanTijd(int ticks) override;
    virtual int   GetAanTijd() override;
    virtual void  SetAanTijdToolTip(const std::string& tooltip) override;

    virtual void  SetUitTijd(int ticks) override;
    virtual int   GetUitTijd() override;
    virtual void  SetUitTijdToolTip(const std::string& tooltip) override;

    virtual void  SetHoogte(int hoogte) override;
    virtual int   GetHoogte() override;
    virtual void  SetHoogteToolTip(const std::string& tooltip) override;

    virtual void  SetBreedte(int breedte) override;
    virtual int   GetBreedte() override;
    virtual void  SetBreedteToolTip(const std::string& tooltip) override;

    // wissels
    virtual void  SetAansluiting(int index, const std::string& aansluiting) override;
    virtual std::string GetAansluiting(int index) override;
    virtual void  SetAansluitingToolTip(int index, const std::string& tooltip) override;

    virtual void  SetLengte(int index, int lengte) override;
    virtual int   GetLengte(int index) override;
    virtual void  SetLengteToolTip(int index, const std::string& tooltip) override;

    virtual void  SetMaxSnelheid(int index, int snelheid) override;
    virtual int   GetMaxSnelheid(int index) override;
    virtual void  SetMaxSnelheidToolTip(int index, const std::string& tooltip) override;


private:
    Ui::QtWisselDialoog *ui;

    std::string m_uitleg;
    float m_adres;
    boost::optional<int> m_hwAan; // detecteerd tussen wissels en IO
    std::string m_hwAanToolTip;
    int m_hwTot;
    std::string m_hwTotToolTip;
    int m_typeIndex;
    std::vector<std::string> m_typeLabels;
    int m_aanTijd;
    std::string m_aanTijdToolTip;
    int m_uitTijd;
    std::string m_uitTijdToolTip;
    int m_hoogte;
    std::string m_hoogteToolTip;
    int m_breedte;
    std::string m_breedteToolTip;
    static const int editFields=4;
    std::array<boost::optional<std::string>,editFields> m_aansluiting;
    std::array<boost::optional<std::string>,editFields> m_aansluitingToolTip;
    std::array<boost::optional<int>,editFields> m_lengte;
    std::array<boost::optional<std::string>,editFields> m_lengteToolTip;
    std::array<boost::optional<int>,editFields> m_maxSnelheid;
    std::array<boost::optional<std::string>,editFields> m_maxSnelheidToolTip;
    std::array<QLineEdit *,editFields> m_editAansluiting;
    std::array<QLineEdit *,editFields> m_editLengte;
    std::array<QLineEdit *,editFields> m_editMaxSnelheid;
    QValidator *m_adresValidator;
    QValidator *m_intValidator;

};

#endif // QTWISSELDIALOOG_H
