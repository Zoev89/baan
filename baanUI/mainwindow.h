#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include "UIBaanLibCreator.h"
#include "QBitmapLabel.h"
#include "UIRegelaars.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, std::string filename, bool editMode);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event) override;



private slots:
    void on_actionOpen_triggered();
    void on_actionNieuweTrein_triggered();
    void on_actionTd_Dump_triggered();
    void on_actionNieuw_IO_triggered();

    void on_actionNieuw_Blok_triggered();

    void on_actionCheck_triggered();

    void on_actionView_Messages_triggered();

    void on_actionIO_Overzicht_triggered();

    void on_actionBaan_Overzicht_triggered();

private:
    Ui::MainWindow *ui;
    QBitmapLabel *bitMap;
    UIRegelaars *mRegelaars;

    std::unique_ptr<UIBaanLibCreator> mBaanCreator;
    bool m_editMode;
};

#endif // MAINWINDOW_H
