#ifndef QTKOPRICHTINGDIALOOG_H
#define QTKOPRICHTINGDIALOOG_H

#include <QDialog>
#include <QValidator>
#include "IKopRichtingDialoog.h"

namespace Ui {
class QtKopRichtingDialoog;
}

class QtKopRichtingDialoog : public QDialog, public IKopRichtingDialoog
{
    Q_OBJECT

public:
    explicit QtKopRichtingDialoog(QWidget *parent = 0);
    ~QtKopRichtingDialoog();

    // IKopRichtingDialoog
    virtual bool RunDialogOk() override; // returns true when ok is pressed

    virtual void  SetKopBlok(int blok) override;
    virtual int  GetKopBlok() override;
    virtual void  SetLengte(int lengte) override;
    virtual int  GetLengte() override;
    virtual void  SetKopBlokMee(bool check) override;
    virtual bool  GetKopBlokMee() override;
    virtual void  SetVooruitTerug(bool sliderVT) override;
    virtual bool  GetVoorUitTerug() override;
    virtual void SetErrorText(const std::string& errorText) override;
    virtual std::string GetErrorText() override;

private:
    Ui::QtKopRichtingDialoog *ui;
    QValidator *m_kopBlokValidator;
    QValidator *m_lengteValidator;

    int m_kopBlok;
    int m_lengte;
    bool m_kopBlokMee;
    bool m_vt;
    std::string m_errorText;
};

#endif // QTKOPRICHTINGDIALOOG_H
