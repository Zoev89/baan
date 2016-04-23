#ifndef QTADDBLOKDIALOOG_H
#define QTADDBLOKDIALOOG_H
#include "IAddBlokDialoog.h"
#include <QDialog>
#include <QValidator>

namespace Ui {
class QtAddBlokDialoog;
}

class QtAddBlokDialoog : public QDialog, public IAddBlokDialoog
{
    Q_OBJECT

public:
    explicit QtAddBlokDialoog(QWidget *parent = 0);
    ~QtAddBlokDialoog();

    virtual bool RunDialogOk() override; // returns true when ok is pressed

    virtual void  SetVrijBlok(int blok) override;
    virtual int  GetVrijBlok() override;

private:
    Ui::QtAddBlokDialoog *ui;
    QValidator *m_blokValidator;
    int m_blokNummer;

};

#endif // QTADDBLOKDIALOOG_H
