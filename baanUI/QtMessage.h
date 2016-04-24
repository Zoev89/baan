#ifndef QTMESSAGE_H
#define QTMESSAGE_H
#include "IMessage.h"

#include <QDialog>

namespace Ui {
class QtMessage;
}

class QtMessage : public QDialog, public IMessage
{
    Q_OBJECT

public:
    explicit QtMessage(QWidget *parent = 0);
    ~QtMessage();

    virtual void message(const std::string msg) override;

    void closeDialog();

Q_SIGNALS:
    void sendMessageSignal(const QString);

public Q_SLOTS:
    void displayMessageSlot(const QString);


private slots:
    void on_clear_clicked();

private:
    Ui::QtMessage *ui;
    bool closing;
};

#endif // QTMESSAGE_H
