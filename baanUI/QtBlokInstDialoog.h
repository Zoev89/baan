#ifndef QTBLOKINSTDIALOOG_H
#define QTBLOKINSTDIALOOG_H

#include <QDialog>
#include <QValidator>
#include "IBlokInst.h"

namespace Ui {
class QtBlokInstDialoog;
}

class QtBlokInstDialoog : public QDialog, public IBlokInst
{
    Q_OBJECT

public:
    explicit QtBlokInstDialoog(QWidget *parent = 0);
    ~QtBlokInstDialoog();


    //IBlokInst
    virtual bool RunDialogOk() override; // returns true when ok is pressed

    virtual void  SetBlok(int blokNummer) override;
    virtual int   GetBlok() override;
    virtual void  SetVolgendBlok(const std::string& blok) override;
    virtual std::string GetVolgendBlok() override;
    virtual void  SetMaxSnelheid(int maxSnelheid) override;
    virtual int   GetMaxSnelheid() override;
    virtual void  SetLengte(int lengte) override;
    virtual int   GetLengte() override;
    virtual void  SetBovenLeiding(bool bovenleiding) override;
    virtual bool  GetBovenLeiding() override;
    virtual void  SetSein(int seinIndex) override;
    virtual int   GetSein() override;
    virtual void  SetVoorKeurRichting(int voorKeurRichtingIndex) override;
    virtual int   GetVoorKeurRichting() override;


private:
    Ui::QtBlokInstDialoog *ui;
    QValidator *m_blokValidator;
    QValidator *m_intValidator;
    QValidator *m_maxSnelheidValidator;
    int m_blokNummer;
    std::string m_volgendBlok;
    int m_maxSnelheid;
    int m_lengte;
    bool m_bovenleiding;
    int m_seinIndex;
    int m_voorKeurRichtingIndex;

};

#endif // QTBLOKINSTDIALOOG_H
