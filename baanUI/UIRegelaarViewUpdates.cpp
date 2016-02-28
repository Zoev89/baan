#include "UIRegelaarViewUpdates.h"

UIRegelaarViewUpdates::UIRegelaarViewUpdates(QWidget *parent):
    ::QWidget(parent)
{
}

void UIRegelaarViewUpdates::NieuweRegelaar(int regelaarNummer, const std::string &bitmapFileName)
{
    emit NieuweRegelaarSignal(regelaarNummer, QString(bitmapFileName.c_str()));
}

void UIRegelaarViewUpdates::SnelheidProgress(int regelaarNummer, int snelheid)
{
    emit SnelheidProgressSignal(regelaarNummer, snelheid);
}

void UIRegelaarViewUpdates::Meldingen(int regelaarNummer, const std::string &meldingText)
{
    emit MeldingenSignal(regelaarNummer, QString(meldingText.c_str()));
}

void UIRegelaarViewUpdates::NewSnelheid(int regelaarNummer, int snelheid)
{
    emit NewSnelheidSignal(regelaarNummer, snelheid);
}

void UIRegelaarViewUpdates::RichtingVerandering(int regelaarNummer, int richting)
{
    emit RichtingVeranderingSignal(regelaarNummer, richting);
}

void UIRegelaarViewUpdates::IgnoreStop(int regelaarNummer, bool ignore)
{
    emit IgnoreStopSignal(regelaarNummer, ignore);
}

void UIRegelaarViewUpdates::ShowRegelaar(int regelaarNummer, bool show)
{
    emit ShowRegelaarSignal(regelaarNummer, show);
}
