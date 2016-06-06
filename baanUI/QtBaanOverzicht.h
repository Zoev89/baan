#ifndef QTBAANOVERZICHT_H
#define QTBAANOVERZICHT_H

#include <QDialog>

namespace Ui {
class QtBaanOverzicht;
}

class QtBaanOverzicht : public QDialog
{
    Q_OBJECT

public:
    explicit QtBaanOverzicht(QWidget *parent = 0);
    ~QtBaanOverzicht();
    void Display(const std::vector<std::string>& list);
    void closeDialog();

private:
    Ui::QtBaanOverzicht *ui;
};

#endif // QTBAANOVERZICHT_H
