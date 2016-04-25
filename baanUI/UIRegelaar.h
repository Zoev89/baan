#ifndef UIREGELAAR_H
#define UIREGELAAR_H

#include <QDialog>
#include "ui_UIRegelaar.h"

//namespace Ui {
//class UIRegelaar;
//}

class UIRegelaar : public QDialog
{
    Q_OBJECT

public:
    explicit UIRegelaar(QWidget *parent = 0);
    ~UIRegelaar();

Q_SIGNALS:
    void snelheidSignal(int value);
    void vtValueSignal(int value);
    void verwijderClickedSignal();
    void ignoreStopSignal(bool checked);
    void instellingenDialogSignal();

public Q_SLOTS:
    void vtValue(int);
    void snelheid(int);

public:
    Ui::UIRegelaar *ui;
private slots:
    void on_verwijder_clicked();

    void on_ignoreStop_clicked(bool checked);

    void on_bitmap_clicked();

private:
};

#endif // UIREGELAAR_H
