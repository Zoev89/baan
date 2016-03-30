#ifndef UIREGELAARS_H
#define UIREGELAARS_H
#include "baanLib.h"
#include "UIRegelaar.h"

#include <array>
#include <cstddef>
#include <QWidget>
#include <QPixmap>

class UIRegelaars : public QWidget
{
    Q_OBJECT
public:
    explicit UIRegelaars(QWidget *parent,BaanLib * baanLib);
    QPixmap* GetBitmap(int regelaarNummer);

signals:

public slots:
    // IMainWindowDrawing
    void NieuweRegelaarSlot(int regelaarNummer, const QString bitmapFileName);
    void SnelheidProgressSlot(int regelaarNummer, int snelheid);
    void MeldingenSlot(int regelaarNummer, const QString meldingText);
    void NewSnelheidSlot(int regelaarNummer, int snelheid);
    void RichtingVeranderingSlot(int regelaarNummer, int richting);
    void IgnoreStopSlot(int regelaarNummer, bool ignore);
    void ShowRegelaarSlot(int regelaarNummer, bool show);

    // eigen vanaf de UIRegelaar
    void snelheidSlot(int value);
    void vtValueSlot(int value);
    void verwijderClicked();
    void ignoreStopChangedSlot(bool ignore);

private:
    void updateRegelaar(int regelaarNummer);

    BaanLib *mBaanLib;
    UIRegelaar *mRegelaar;
    int mActiveRegelaar;
    struct RegelaarData
    {
        RegelaarData()
            : bitmap(nullptr)
            , snelheid(0)
            , richting(0)
            , snelheidProgess(0)
            , ignoreStop(false)
        {

        }

        ~RegelaarData()
        {
            if (bitmap) delete bitmap;
        }

        QPixmap *bitmap;
        int snelheid;
        int richting;
        int snelheidProgess;
        bool ignoreStop;
    };

    std::array<RegelaarData, MAX_AANTAL_REGELAARS>  mRegelaars;
};

#endif // UIREGELAARS_H
