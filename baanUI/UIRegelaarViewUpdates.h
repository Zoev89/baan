#ifndef UIREGELAARVIEWUPDATES_H
#define UIREGELAARVIEWUPDATES_H
#include "IRegelaarViewUpdates.h"
#include <QString>
#include <QWidget>

class UIRegelaarViewUpdates : public QWidget,  public IRegelaarViewUpdates
{
    Q_OBJECT
public:
    UIRegelaarViewUpdates(QWidget *parent);

    // IRegelaarViewUpdates
    virtual void NieuweRegelaar(int regelaarNummer, const std::string& bitmapFileName);
    virtual void SnelheidProgress(int regelaarNummer, int snelheid);
    virtual void Meldingen(int regelaarNummer, const std::string& meldingText);
    virtual void NewSnelheid(int regelaarNummer, int snelheid);
    virtual void RichtingVerandering(int regelaarNummer, int richting);
    virtual void IgnoreStop(int regelaarNummer, bool ignore);
    virtual void ShowRegelaar(int regelaarNummer, bool show);

Q_SIGNALS:
    void NieuweRegelaarSignal(int regelaarNummer, const QString bitmapFileName);
    void SnelheidProgressSignal(int regelaarNummer, int snelheid);
    void MeldingenSignal(int regelaarNummer, const QString meldingText);
    void NewSnelheidSignal(int regelaarNummer, int snelheid);
    void RichtingVeranderingSignal(int regelaarNummer, int richting);
    void IgnoreStopSignal(int regelaarNummer, bool ignore);
    void ShowRegelaarSignal(int regelaarNummer, bool show);
private:

};


#endif // UIREGELAARVIEWUPDATES_H
