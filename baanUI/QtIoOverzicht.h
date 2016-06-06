#ifndef QTIOOVERZICHT_H
#define QTIOOVERZICHT_H

#include <QDialog>

namespace Ui {
class QtIoOverzicht;
}

class QtIoOverzicht : public QDialog
{
    Q_OBJECT

public:
    explicit QtIoOverzicht(QWidget *parent = 0);
    ~QtIoOverzicht();
    void Display(const std::vector<std::string>& list);
    void closeDialog();

private:
    Ui::QtIoOverzicht *ui;
};

#endif // QTIOOVERZICHT_H
