#include "QtMessage.h"
#include "ui_QtMessage.h"
#include <iostream>

QtMessage::QtMessage(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QtMessage)
    , closing(false)

{
    ui->setupUi(this);
    connect(this, SIGNAL(sendMessageSignal(const QString)), this, SLOT(displayMessageSlot(const QString)));
}

QtMessage::~QtMessage()
{
    delete ui;
}

void QtMessage::message(const std::string msg)
{
    if (!closing)
    {
        emit sendMessageSignal(msg.c_str());
    }
}

void QtMessage::displayMessageSlot(const QString msg)
{
    new QListWidgetItem(msg, ui->listLijst);
    if (closing) hide();
    else show();
}

void QtMessage::closeDialog()
{
    closing = true;
    hide();
}

void QtMessage::on_clear_clicked()
{
    ui->listLijst->clear();
}

