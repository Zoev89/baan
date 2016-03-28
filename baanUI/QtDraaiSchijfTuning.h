#ifndef QTDRAAISCHIJFTUNING_H
#define QTDRAAISCHIJFTUNING_H
#include <QDialog>
#include "baanLib.h"

#include "IDraaiSchijfTuning.h"

namespace Ui {
class QtDraaiSchijfTuning;
}

class QtDraaiSchijfTuning : public QDialog, public IDraaiSchijfTuning
{
    Q_OBJECT

public:
    explicit QtDraaiSchijfTuning(QWidget *parent = 0);
    virtual void show(int basisAdres) override;
    void setBaanLib(BaanLib * baanLib);

    ~QtDraaiSchijfTuning();

private slots:
    void on_min_1_clicked();

    void on_plus_1_clicked();

    void on_plus_2_clicked();

    void on_plus_4_clicked();

    void on_plus_8_clicked();

    void on_plus_16_clicked();

    void on_plus_32_clicked();

    void on_min_2_clicked();

    void on_min_4_clicked();

    void on_min_8_clicked();

    void on_min_16_clicked();

    void on_min_32_clicked();

    void on_SaveInEprom_clicked();

private:
    Ui::QtDraaiSchijfTuning *ui;
    int mBasisAdres;
    BaanLib * mBaanLib;
};

#endif // QTDRAAISCHIJFTUNING_H
