#ifndef QTDRAAIAANSLUITING_H
#define QTDRAAIAANSLUITING_H
#include "IWisselDialoog.h"
#include <QValidator>


#include <QDialog>

namespace Ui {
class QtDraaiAansluiting;
}

class QtDraaiAansluiting : public QDialog
{
    Q_OBJECT

public:
    explicit QtDraaiAansluiting(QWidget *parent = 0);
    ~QtDraaiAansluiting();
    void SetValues(const DraaiSchijfAansluiting& waardes);
    DraaiSchijfAansluiting GetValues();

private:
    Ui::QtDraaiAansluiting *ui;
    QValidator *m_aansluitingValidator;
    QValidator *m_blokValidator;

};

#endif // QTDRAAIAANSLUITING_H
