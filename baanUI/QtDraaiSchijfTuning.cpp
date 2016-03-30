#include "QtDraaiSchijfTuning.h"
#include "ui_QtDraaiSchijfTuning.h"

QtDraaiSchijfTuning::QtDraaiSchijfTuning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtDraaiSchijfTuning)
{
    ui->setupUi(this);
}

QtDraaiSchijfTuning::~QtDraaiSchijfTuning()
{
    delete ui;
}

void QtDraaiSchijfTuning::show(int adres)
{
    mAdres = adres;
    QWidget::show();
}

void QtDraaiSchijfTuning::setBaanLib(BaanLib * baanLib)
{
    mBaanLib = baanLib;
}

void QtDraaiSchijfTuning::on_plus_1_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48, false);
}

void QtDraaiSchijfTuning::on_plus_2_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 1*2, false);
}

void QtDraaiSchijfTuning::on_plus_4_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 2*2, false);
}

void QtDraaiSchijfTuning::on_plus_8_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 3*2, false);
}

void QtDraaiSchijfTuning::on_plus_16_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 4*2, false);
}

void QtDraaiSchijfTuning::on_plus_32_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 5*2, false);
}

void QtDraaiSchijfTuning::on_min_1_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 1, false);
}

void QtDraaiSchijfTuning::on_min_2_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 1*2 + 1, false);
}

void QtDraaiSchijfTuning::on_min_4_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 2*2 + 1, false);
}

void QtDraaiSchijfTuning::on_min_8_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 3*2 + 1, false);
}

void QtDraaiSchijfTuning::on_min_16_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 4*2 + 1, false);
}

void QtDraaiSchijfTuning::on_min_32_clicked()
{
    mBaanLib->HardwareReq(mAdres, 48 + 5*2 + 1, false);
}

void QtDraaiSchijfTuning::on_SaveInEprom_clicked()
{
    mBaanLib->HardwareReq(mAdres, 62, false);
}
