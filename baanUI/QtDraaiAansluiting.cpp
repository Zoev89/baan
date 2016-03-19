#include "QtDraaiAansluiting.h"
#include "ui_QtDraaiAansluiting.h"

QtDraaiAansluiting::QtDraaiAansluiting(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtDraaiAansluiting)
    , m_blokValidator(new QIntValidator(-1, 1023, this))
    , m_aansluitingValidator(new QIntValidator(0, 47, this))

{
    ui->setupUi(this);
    ui->aansluiting->setValidator(m_aansluitingValidator);
    ui->blok->setValidator(m_blokValidator);
}

QtDraaiAansluiting::~QtDraaiAansluiting()
{
    delete ui;
}

void QtDraaiAansluiting::SetValues(const DraaiSchijfAansluiting& waardes)
{
    ui->aansluiting->setText(std::to_string(waardes.aansluitingNummer).c_str());
    ui->blok->setText(std::to_string(waardes.blok).c_str());
    ui->blokRichting->setChecked(waardes.richtingVooruit);
    ui->gndFirst->setChecked(waardes.gndFirst);
}

DraaiSchijfAansluiting QtDraaiAansluiting::GetValues()
{
    DraaiSchijfAansluiting waardes;
    waardes.aansluitingNummer = std::stoi(ui->aansluiting->text().toStdString());
    waardes.blok = std::stoi(ui->blok->text().toStdString());
    waardes.richtingVooruit = ui->blokRichting->isChecked();
    waardes.gndFirst = ui->gndFirst->isChecked();
    return waardes;
}
