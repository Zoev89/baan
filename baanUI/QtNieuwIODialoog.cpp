#include "QtNieuwIODialoog.h"
#include "ui_QtNieuwIODialoog.h"

QtNieuwIODialoog::QtNieuwIODialoog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QtNieuwIODialoog)
{
    ui->setupUi(this);
}

QtNieuwIODialoog::~QtNieuwIODialoog()
{
    delete ui;
}


bool QtNieuwIODialoog::RunDialogOk()
{

}

void QtNieuwIODialoog::SetAdres(float adres)
{

}

float QtNieuwIODialoog::GetAdres()
{

}

void QtNieuwIODialoog::SetBlok(int blok)
{

}

int QtNieuwIODialoog::GetBlok()
{

}

void QtNieuwIODialoog::SetTypeIndex(int index)
{

}

int QtNieuwIODialoog::GetTypeIndex()
{

}

void QtNieuwIODialoog::SetTypeLabels(std::vector<std::string> labels)
{

}
