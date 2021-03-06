#ifndef QTNIEUWIODIALOOG_H
#define QTNIEUWIODIALOOG_H

#include <QDialog>
#include "INieuwIODialoog.h"
#include "QValidator"

namespace Ui {
class QtNieuwIODialoog;
}

class QtNieuwIODialoog : public QDialog, public INieuwIODialoog
{
    Q_OBJECT

public:
    explicit QtNieuwIODialoog(QWidget *parent = 0);
    ~QtNieuwIODialoog();

    virtual bool RunDialogOk() override; // returns true when ok is pressed

    virtual void  SetAdres(float adres) override;
    virtual float GetAdres() override;
    virtual void  SetBlok(int blok) override;
    virtual int   GetBlok() override;

    virtual void  SetTypeIndex(int index) override;
    virtual int   GetTypeIndex() override;
    virtual void  SetTypeLabels(std::vector<std::string> labels) override;

private:
    Ui::QtNieuwIODialoog *ui;
    QValidator *m_adresValidator;
    QValidator *m_blokValidator;
    float m_adres;
    int m_kopBlok;
    int m_typeIndex;

};

#endif // QTNIEUWIODIALOOG_H
