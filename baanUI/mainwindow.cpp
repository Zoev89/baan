#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>


// QT neemt de systeem locale over dus de c libraries gebruiken niet de c locale!
// IK had problemen met 1.5 want dat leest die dan als 1
// Moest de LC_NUMERIC zetten in .bashrc
// export LC_NUMERIC=en_US.UTF-8
// dan is het goed


MainWindow::MainWindow(QWidget *parent, string filename, bool editMode)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_editMode(editMode)
{
    ui->setupUi(this);
    mBaanCreator.reset(new UIBaanLibCreator(this, editMode));
    mRegelaars = new UIRegelaars(this,mBaanCreator->mBaanLib.get());
    bitMap  = new QBitmapLabel(this,mBaanCreator->mBaanLib.get(), mRegelaars);

    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(DrawLineSignal(int, int, int, int, int, int)), bitMap, SLOT(DrawLineSlot(int, int, int, int, int, int)));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(RedrawAllSignal()), bitMap, SLOT(RedrawAllSlot()));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(SetBitmapSignal(const QString)), bitMap, SLOT(SetBitmapSlot(const QString)));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(RedrawRecSignal(int, int, int , int)), bitMap, SLOT(RedrawRecSlot(int, int, int, int)));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(BlokDisplaySignal(int, int, int, int, const QString, int)), bitMap, SLOT(BlokDisplaySlot(int, int, int, int, const QString, int)));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(BlokClearEditModeDisplaySignal(bool, int, int, const QString)), bitMap, SLOT(BlokClearEditModeDisplaySlot(bool, int, int, const QString)));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(DisplayStringSignal(int, int, const QString)), bitMap, SLOT(DisplayStringSlot(int, int, const QString)));
    connect(mBaanCreator->mainWindowDrawing.get(), SIGNAL(VierkantDisplaySignal(int, int, int , int, int)), bitMap, SLOT(VierkantDisplaySlot(int, int, int, int, int)));

    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(NieuweRegelaarSignal(int, const QString)), mRegelaars, SLOT(NieuweRegelaarSlot(int, const QString)));
    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(SnelheidProgressSignal(int, int)), mRegelaars, SLOT(SnelheidProgressSlot(int, int)));
    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(MeldingenSignal(int, const QString)), mRegelaars, SLOT(MeldingenSlot(int, const QString)));
    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(NewSnelheidSignal(int, int)), mRegelaars, SLOT(NewSnelheidSlot(int, int)));
    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(RichtingVeranderingSignal(int, int)), mRegelaars, SLOT(RichtingVeranderingSlot(int, int)));
    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(IgnoreStopSignal(int, bool)), mRegelaars, SLOT(IgnoreStopSlot(int, bool)));
    connect(mBaanCreator->regelaarViewUpdates.get(), SIGNAL(ShowRegelaarSignal(int, bool)), mRegelaars, SLOT(ShowRegelaarSlot(int, bool)));

    setCentralWidget(bitMap);
    if (!filename.empty())
    {
        mBaanCreator->mBaanLib->BaanOpen(filename, m_editMode);
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mBaanCreator->mBaanLib->BaanClose();
    mBaanCreator->message.closeDialog();
    mBaanCreator->ioOverzicht.closeDialog();
    mBaanCreator->baanOverzicht.closeDialog();
    event->accept();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
            tr("Baan Files (*.blk)"));

    if (!fileName.isEmpty()) {
        mBaanCreator->mBaanLib->BaanOpen(fileName.toStdString(), m_editMode);
        setWindowTitle(fileName);
    }
}

void MainWindow::on_actionNieuweTrein_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Trein"), QString(),
            tr("Loc Files (*.loc)"));

    if (!fileName.isEmpty()) {
        mBaanCreator->mBaanLib->BaanRegelaarOpen(fileName.toStdString());
    }
}

void MainWindow::on_actionTd_Dump_triggered()
{
    mBaanCreator->mBaanLib->TdDump();
}

void MainWindow::on_actionNieuw_IO_triggered()
{
    mBaanCreator->mBaanLib->BaanDocNieuwIo();
}

void MainWindow::on_actionNieuw_Blok_triggered()
{
    mBaanCreator->mBaanLib->BaanAddBlok();
}

void MainWindow::on_actionCheck_triggered()
{
    if (mBaanCreator->mBaanLib->BaanCheckDatabase()==0)
    {
        QMessageBox msgBox;
        msgBox.setText("Geen programma fouten");
        msgBox.exec();
    }
}

void MainWindow::on_actionView_Messages_triggered()
{
    mBaanCreator->message.show();
}

void MainWindow::on_actionIO_Overzicht_triggered()
{
    mBaanCreator->ioOverzicht.Display(mBaanCreator->mBaanLib->GetIoOverzicht());
}

void MainWindow::on_actionBaan_Overzicht_triggered()
{
    mBaanCreator->baanOverzicht.Display(mBaanCreator->mBaanLib->GetBaanOverzicht());

}
