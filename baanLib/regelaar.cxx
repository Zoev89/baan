// Baan: A railway model controlling program
// Copyright (C) 1998 - 2001 Eric Kathmann
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA  02111-1307, USA.


#include "baan.h"
#include "baanTypes.h"
#include "regelaar.h"
#include "td.h"
#include "baanSemaphore.h"
#include "baanMessage.h"
#include "baanDoc.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "baanTreinen.h"
#include "baanTime.h"

#define TIC_FREQ       6        /* om de hoeveel moet er een streepje op de regelaar */
#define KORTSLUIT 30

#define REGEL_COUNT_MAX 8

// Update variabelen voor de PID regelaar (snelheid)
#define GAIN_UPDATE 0.002F      //0.0002F


/////////////////////////////////////////////////////////////////////////////
// Regelaar dialog


Regelaar::Regelaar (IRegelaarViewUpdates &regelaarViewUpdates, IBaanTreinen &baanTreinen, IBaanDoc &baanDoc, IRegelaarInstellingenDialoog &regelDialoog, IBaanMessage &baanMessage, ITelefoonConnectie &telefoonConnectie, ITd &td):
    mRegelaarViewUpdates(regelaarViewUpdates),
    mBaanTreinen(baanTreinen),
    mBaanDoc(baanDoc),
    mRegelInstellingenDialoog(regelDialoog),
    mBaanMessage(baanMessage),
    mTelefoonConnectie(telefoonConnectie),
    mTd(td)
{
    Gebruikt = 0;
    m_pRegelAanvraag = NULL;
    InterneSnelheid = 0;
    IgnoreStop = 0;
    HuidigeSnelheid = 0;
    GewensteSnelheid = 0;
    Verandering = 0;
    NormaalAlfa = 0;
    NormaalClip = 0;
    StopAlfa = 0;
    StopClip = 0;
    TotaalAfstand = 0;
    TotaalTicken = 0;
    KortSluitCount = 0;
    ELoc = 0;
    VerlichtingCount = 0;
    MaxSnelheid = 60;
    MinSnelheid = 0;
    Rijden = 0;
    TopSnelheid = 0;
    TopSnelheidKmh = 0;
    blokLengte = 0;
    c1 = 0;
    c2 = 0;
    fNormaalAlfa = 0;
    fStopAlfa = 0;
    gemetenSnelheid = 0;
    k0 = 0;
    k1 = 0;
    kc = 0;
    laatsteWagonDecimatie = 0;
    laatsteWagonDetect = 0;
    regelCount = 0;
    t_d = 0;
    t_i = 0;
    t_s = 0;
    tijd = 0;
    xk1 = 0;


    lastRegelKeuze = 1;
    lastGain1 = 1;
    lastStand1 = 0;
    lastGain2 = 1;
    lastStand2 = 1;
    helling = 1;
    dodeTijd = 6;
    k_lpf = 0.9;
    plusMinus = 8;
    RegelaarNummer = 0;
    initPid ();
}

void Regelaar::SetSnelheid(int snelheid)
{
    GewensteSnelheid = snelheid;
    Verandering = 1;
    mTelefoonConnectie.SetActiveRegelaar(RegelaarNummer,Richting,snelheid);
}

int Regelaar::GetSnelheid()
{
    return getGewensteSnelheid();
}

void Regelaar::SetRichting(int vt)
{
    Richting = vt;
    mTelefoonConnectie.SetActiveRegelaar(RegelaarNummer,Richting,getGewensteSnelheid());
}

int Regelaar::GetRichting()
{
    return Richting;
}

void Regelaar::SetIgnoreStop(bool ignore)
{
    IgnoreStop = ignore;
}

bool Regelaar::GetIgnoreStop()
{
    return (IgnoreStop != 0);
}

void Regelaar::RunProgramClicked(bool run)
{
    int temp;

    temp = programRunning;
    programRunning = run;
    if (temp != programRunning)
    {
        if (programRunning)
        {
            // zend een message naar het user programma
            mBaanMessage.Post (WM_BLOK_EVENT, RegelaarNummer,
                             pKopBlok->pBlok->hardwareAdres, 0);
        }
        else
        {
            // zend een message naar het user programma om aftesluiten
            mBaanMessage.Post (WM_BLOK_EVENT, RegelaarNummer, -1, 0);
        }
    }
}

void Regelaar::VerwijderClicked()
{
    CleanUp();
}

void Regelaar::PropertiesClicked()
{
    int totaal, uren, minuten, ticken;
    int afstand;
    char string[20];

    mRegelInstellingenDialoog.SetLocType(locSoort);
    mRegelInstellingenDialoog.SetMaxSnelheid(MaxSnelheid);
    mRegelInstellingenDialoog.SetMinSnelheid(MinSnelheid);
    mRegelInstellingenDialoog.SetTopSnelheid(TopSnelheidKmh);
    mRegelInstellingenDialoog.SetLengte(Lengte);
    afstand = (TotaalAfstand + 50) / 100;
    mRegelInstellingenDialoog.SetTotaalAfstand(afstand);
    mRegelInstellingenDialoog.SetStandUitleg("Stand van de regelaar waarbij de trein x cm (afstand) nodig heeft om tot stilstand te komen.");
    mRegelInstellingenDialoog.SetAfstand1(mAfstand1);
    mRegelInstellingenDialoog.SetStand1(mStand1);
    mRegelInstellingenDialoog.SetAfstand2(mAfstand2);
    mRegelInstellingenDialoog.SetStand2(mStand2);
    mRegelInstellingenDialoog.SetAlphaUitleg("Apha is een gain factor tussen 0.0 (langzaam) en 1.0 (snel). Typical 0.05.");
    mRegelInstellingenDialoog.SetAlphaRijden(fNormaalAlfa);
    mRegelInstellingenDialoog.SetClipUitleg("Clip word gebruikt om te grote veranderingen tegen te gaan [0..63]. Typical 16.");
    mRegelInstellingenDialoog.SetAlphaStoppen(fStopAlfa);
    mRegelInstellingenDialoog.SetClipRijden(NormaalClip >> SNELHEID_SHIFT);
    mRegelInstellingenDialoog.SetClipStoppen(StopClip >> SNELHEID_SHIFT);
    mRegelInstellingenDialoog.SetEloc(ELoc==1);
    mRegelInstellingenDialoog.SetLaatsteWagonCheck(laatsteWagonCheck==1);
    {
        //char *p;
        //p = strrchr (programmaNaam, '/');
        //if (p)
//            p++;
        //if (p)
        //mRegelInstellingenDialoog.SetProgrammaNaam(p);
        mRegelInstellingenDialoog.SetProgrammaNaam(programmaNaam);
        //else
            //mRegelInstellingenDialoog.SetProgrammaNaam("");
    }
    mRegelInstellingenDialoog.SetHerlaadProgramma(false);
    mRegelInstellingenDialoog.SetLangzaamRijdenUitleg("De snelheid setting voor rijden tijdens het uitvoeren van het programma.");

    mRegelInstellingenDialoog.SetLangzaam(Langzaam);
    mRegelInstellingenDialoog.SetRijden(Rijden);

    if (laatsteWagonCheckTotal == 0)
    {
        sprintf (string, "0%%");
    }
    else
    {
        sprintf (string, "%3d%%",
                 (int) ((float) laatsteWagonCheckError /
                        (float) laatsteWagonCheckTotal * 100 + 0.5));
    }
    mRegelInstellingenDialoog.SetErrors(string);
    mRegelInstellingenDialoog.SetLastRegelKeuze(lastRegelKeuze);

    mRegelInstellingenDialoog.SetKlpfUitleg("Gain van de gemeten waarde (K factor) tot de range van de regelaar.");
    mRegelInstellingenDialoog.SetHellingUitleg(" De gain van de I actie van de PID regelaar [0..1.0]. Kleine alpha een langzame regeling.\nZet alpha op 0 voor het uitstakelen van de lastafhankelijke regeling.");
    mRegelInstellingenDialoog.SetDodeTijdUitleg("De gain van de D actie van de PID regelaar [0..1.0]??. Kleine difmul geen differentie actie.");
    mRegelInstellingenDialoog.SetPlusMinusUitleg("Clip waarde van de correcties die de last afhankelijke regeling mag maken.");
    mRegelInstellingenDialoog.SetKLpf(k_lpf);
    mRegelInstellingenDialoog.SetHelling(helling);
    mRegelInstellingenDialoog.SetDodeTijd(dodeTijd);
    mRegelInstellingenDialoog.SetPlusMinus(plusMinus);
    mRegelInstellingenDialoog.SetLastStand1(lastStand1);
    mRegelInstellingenDialoog.SetLastGain1(lastGain1);
    mRegelInstellingenDialoog.SetLastStand2(lastStand2);
    mRegelInstellingenDialoog.SetLastGain2(lastGain2);

    /* Bereken het aantal uren en minuten in bedrijf */
    totaal = TotaalTicken;
    // het aantal ticken per seconde
    ticken = 1000 / (CYCLETIME * 2);
    uren = totaal / (3600 * ticken);
    minuten = (totaal - uren * 3600 * ticken + 30 * ticken) / (60 * ticken);
    sprintf (string, "%d:%02d", uren, minuten);
    mRegelInstellingenDialoog.SetTotaalTijd(string);


    if (mRegelInstellingenDialoog.RunDialogOk())
    {
        int NieuwUren, NieuwMinuten;

        MaxSnelheid = mRegelInstellingenDialoog.GetMaxSnelheid();
        MinSnelheid = mRegelInstellingenDialoog.GetMinSnelheid();
        if (MaxSnelheid < MinSnelheid)
        {
            // swap de min en max
            int x;
            x = MinSnelheid;
            MinSnelheid = MaxSnelheid;
            MaxSnelheid = x;
        }

        //view.snelheid->range (0, MaxSnelheid);
        //view.snelheid2->range (0, MaxSnelheid);
        //view.gemeten->range (0, MaxSnelheid);

        if (GewensteSnelheid > MaxSnelheid)
        {
            GewensteSnelheid = MaxSnelheid;
        }

        /* test op verandering in afstand en tijd */
        if (mRegelInstellingenDialoog.GetTotaalAfstand() != afstand)
        {
            TotaalAfstand = mRegelInstellingenDialoog.GetTotaalAfstand() * 100;  /* eenheid in cm */
        }
        sscanf (mRegelInstellingenDialoog.GetTotaalTijd().c_str(), "%d:%d", &NieuwUren,
                &NieuwMinuten);
        if ((NieuwUren != uren) || (NieuwMinuten != minuten))
        {
            TotaalTicken = (NieuwUren * 3600 + NieuwMinuten * 60) * ticken;
        }
        ELoc = mRegelInstellingenDialoog.GetEloc();
        laatsteWagonCheck = mRegelInstellingenDialoog.GetLaatsteWagonCheck();
        TopSnelheidKmh = mRegelInstellingenDialoog.GetTopSnelheid();
        TopSnelheid = (int) (63.0 / (float) TopSnelheidKmh * 16384 + 0.5);
        Lengte = mRegelInstellingenDialoog.GetLengte();

        fNormaalAlfa = (float) mRegelInstellingenDialoog.GetAlphaRijden();
        fStopAlfa = (float) mRegelInstellingenDialoog.GetAlphaStoppen();
        NormaalClip = mRegelInstellingenDialoog.GetClipRijden() << SNELHEID_SHIFT;
        StopClip = mRegelInstellingenDialoog.GetClipStoppen() << SNELHEID_SHIFT;

        NormaalAlfa = (int) (fNormaalAlfa * 32768);
        StopAlfa = (int) (fStopAlfa * 32768);

        InitRemWeg ((float) mRegelInstellingenDialoog.GetStand1(),
                    (float) mRegelInstellingenDialoog.GetAfstand1(),
                    (float) mRegelInstellingenDialoog.GetStand2(),
                    (float) mRegelInstellingenDialoog.GetAfstand2());


        lastRegelKeuze = mRegelInstellingenDialoog.GetLastRegelKeuze();
        k_lpf = (float) mRegelInstellingenDialoog.GetKLpf();
        helling = (float) mRegelInstellingenDialoog.GetHelling();
        dodeTijd = (float) mRegelInstellingenDialoog.GetDodeTijd();
        plusMinus = mRegelInstellingenDialoog.GetPlusMinus();
        lastStand1 = mRegelInstellingenDialoog.GetLastStand1();
        lastStand2 = mRegelInstellingenDialoog.GetLastStand2();
        lastGain1 = (float) mRegelInstellingenDialoog.GetLastGain1();
        lastGain2 = (float) mRegelInstellingenDialoog.GetLastGain2();

        initPid ();
        // reset de error meeting
        laatsteWagonCheckError = 0;
        laatsteWagonCheckTotal = 0;
        herlaadProgramma = (int)mRegelInstellingenDialoog.GetHerlaadProgramma();
        Langzaam = mRegelInstellingenDialoog.GetLangzaam();
        Rijden = mRegelInstellingenDialoog.GetRijden();
        if (herlaadProgramma)
        {
            if (mRegelInstellingenDialoog.GetProgrammaNaam().c_str()[0] != 0)
            {
                std::cout << mRegelInstellingenDialoog.GetProgrammaNaam() << std::endl;
                strcpy (programmaNaam, mRegelInstellingenDialoog.GetProgrammaNaam().c_str());
            }
            mBaanDoc.baanDocHerlaadProgramma (RegelaarNummer);
            programRunning = 0;
            RunProgramClicked(true);
        }
        strcpy (locSoort, mRegelInstellingenDialoog.GetLocType().c_str());
    }

}

std::string Regelaar::GetBitmapFileName()
{
    return bitmapName;
}

void Regelaar::Show(bool show)
{
    mRegelaarViewUpdates.ShowRegelaar(RegelaarNummer, show);
}



// wordt gebruikt vanuit het programma besturing
// return de actuele stand
int
Regelaar::zetGewensteSnelheid (int snelheid)
{
    int ret;
    if (snelheid > MaxSnelheid)
    {
        snelheid = MaxSnelheid;
    }
    if (snelheid <= 0)
    {
        snelheid = 0;
    }
    ret = GewensteSnelheid;
    GewensteSnelheid = snelheid;
    mRegelaarViewUpdates.NewSnelheid(RegelaarNummer, snelheid);
    return ret;
}

void
Regelaar::Sneller ()
{
    if (GewensteSnelheid < MaxSnelheid)
    {
        GewensteSnelheid += 1;
        Verandering = 1;
        //      m_Snelheid.SetPos (MaxSnelheid - GewensteSnelheid);

    }
}

void
Regelaar::Langzamer ()
{
    if (GewensteSnelheid > 0)
    {
        GewensteSnelheid -= 1;
        Verandering = 1;
        //      m_Snelheid.SetPos (MaxSnelheid - GewensteSnelheid);
    }
}

void
Regelaar::HandRegelaarStop ()
{
    /* Noodstop door gegeven via de hand control */
    if (GewensteSnelheid != 0)
    {
        //      m_Snelheid.SetPos (MaxSnelheid);
    }
    GewensteSnelheid = 0;
    Verandering = 1;
}

void
Regelaar::RichtingVerandering ()
{
    if (Richting)
    {
        Richting = 0;
    }
    else
    {
        Richting = 1;
    }
    mRegelaarViewUpdates.RichtingVerandering(RegelaarNummer, Richting);
}

// Ik gebruik lineaire interpolatie/extrapolatie tussen de gegeven punten
// Heb ooit een kwadratische functie (curve fitting) gebruikt maar met de
// gegeven input bleek dat soms een negative curve ontstond voor de hoge waardes.
// Dat is natuurlijk compleet fout dus vandaar dat ik nu linearie fitting gebruik
// daarvan kan ik garanderen dat bij toenemende snelheid de afstand groter wordt.
void
Regelaar::InitRemWeg (float Snelheid1, float Afstand1, float Snelheid2,
                      float Afstand2)
{
    float hoek1, hoek2;
    int i;

    if (Snelheid1 > Snelheid2)
    {
        // Voor het geval iemand de waardes niet oplopend invult zorg ik dat het nu wel goed
        // gaat.
        i = Snelheid1;
        Snelheid1 = Snelheid2;
        Snelheid2 = i;
        i = Afstand1;
        Afstand1 = Afstand2;
        Afstand2 = i;
    }

    mStand1 = (int) Snelheid1;
    mAfstand1 = (int) Afstand1;
    mStand2 = (int) Snelheid2;
    mAfstand2 = (int) Afstand2;

    hoek1 = (Snelheid1 == 0.0) ? 0.0 : Afstand1 / Snelheid1;
    hoek2 = ((Snelheid2 - Snelheid1) == 0.0) ? 0.0 : (Afstand2 - Afstand1) / (Snelheid2 - Snelheid1);

    for (i = 0; i < 64; i++)
    {
        int lengte;
        
        if (i<Snelheid1)
        {
            lengte = (int) (((hoek1 == 0.0f) ? hoek2: hoek1) * i);
        }
        else
        {
            lengte = (int) (((hoek2 <= 0.0f) ? hoek1: hoek2) * i + Afstand1);
        }
        RemWegLengte[i] = lengte;
    }
}



void
Regelaar::OnInitDialog (int regelNmr)
{
    int i;

    for (i = 0; i < MAX_NOBLOKS; i++)
    {
        aantalFouten[i] = 0;
    }
    RegelaarNummer = regelNmr;
    InterneSnelheid = 0;
    IgnoreStop = 0;
    HuidigeSnelheid = 0;
    GewensteSnelheid = 0;
    Verandering = 0;
    KortSluitCount = 0;
    VerlichtingCount = 0;
    regelCount = 0;
    decimatie = 0;
    laatsteWagonCheck = 1;
    laatsteWagonCheckError = 0;
    laatsteWagonCheckTotal = 0;
    laatsteWagonDetect = 0;
    laatsteWagonDecimatie = 0;
    herlaadProgramma = 0;
    programRunning = 0;
    Langzaam = 0;
    Rijden = 0;

    vorigeSnelheid = 0;
    initPid ();

    // TODO misschien later een algemene routine om de view te initializeren
    // zodat ik met 1 view alle regelaars kan afhandelen
    // bitmap is nog ergens anders gezet... in BaanDoc
    mRegelaarViewUpdates.NieuweRegelaar(RegelaarNummer, bitmapName);
    mRegelaarViewUpdates.NewSnelheid(RegelaarNummer, GewensteSnelheid);
    mRegelaarViewUpdates.RichtingVerandering(RegelaarNummer, Richting);
    // einde view init

    tijd = baanTimeGetInms ();
    blokLengte = 0;

    return;                       // return TRUE unless you set the focus to a control
}
void Regelaar::SnelheidProgress(int snelheid)
{
    mRegelaarViewUpdates.SnelheidProgress(RegelaarNummer, snelheid);
}

void
Regelaar::CleanUp (void)
{
    FILE *file;

    // PRECONDITION de workthread moet nog operationeel zijn anders hebben
    // we een deadlock!
    // Het is wel zo dat als het programma afgesloten wordt de semWacht al deleted
    // is. De CALLER moet deze semWacht dus NULL maken voor de aanroep van CleanUp()

    Gebruikt = 0;


    /*
   ** Geef aan dat deze regelaar zijn blokken vrijgegeven moeten
   ** worden.
   **/
    if (m_pRegelAanvraag->semWacht)
    {
        m_pRegelAanvraag->RegelaarNummer = RegelaarNummer;
        m_pRegelAanvraag->VerwijderRegelaar = 1;
        m_pRegelAanvraag->Aanvraag = 1;

        baanSemaphoreDown (m_pRegelAanvraag->semWacht);
    }

    file = fopen (RegelaarFileName, "wb");

    if (file != NULL)
    {
        fprintf (file, "#Version\n%d\n", 2);
        fprintf (file, "#ELoc\n%d\n", ELoc);
        fprintf (file, "#MinSnelheid MaxSnelheid\n%d %d\n", MinSnelheid,
                 MaxSnelheid);
        fprintf (file, "#TopSnelheidKmh\n%d\n", TopSnelheidKmh);
        fprintf (file, "#fNormaalAlfa NormaalClip\n%g %d\n", fNormaalAlfa,
                 NormaalClip >> SNELHEID_SHIFT);
        fprintf (file, "#fStopAlfa StopClip\n%g %d\n", fStopAlfa,
                 StopClip >> SNELHEID_SHIFT);
        fprintf (file, "#Snelheid1 Afstand1\n%d %d\n", mStand1, mAfstand1);
        fprintf (file, "#Snelheid2 Afstand2\n%d %d\n", mStand2, mAfstand2);
        fprintf (file, "Programma = %s\n", programmaNaam);
        fprintf (file, "#Langzaam Rijden\n%d %d\n", Langzaam, Rijden);
        fprintf (file, "#locSoort\n%s\n", locSoort);
        fprintf (file, "#laatsteWagonCheck\n%d\n", laatsteWagonCheck);
        fprintf (file,
                 "#lastRegelKeuze lastGain1 lastGain2 lastStand1 lastStand2\n%d %f %f %d %d\n",
                 lastRegelKeuze, lastGain1, lastGain2, lastStand1, lastStand2);
        fprintf (file, "#lowpass plusMinus helling dodeTijd\n%f %d %f %f\n",
                 k_lpf, plusMinus, helling, dodeTijd);
        fprintf (file, "#TotaalAfstand TotaalTicken\n%d %d\n", TotaalAfstand,
                 TotaalTicken);
        fclose (file);
    }

    mBaanTreinen.baanVerwijderTrein (RegelaarNummer);
}


// TODO ontkoppel de ignoreStop->value call naar de message queue
// Hij wordt vanuit de realtime thread aangeroepen
void
Regelaar::WisselIgnoreStop ()
{
    if (IgnoreStop == 0)
    {
        IgnoreStop = 1;
    }
    else
    {
        IgnoreStop = 0;
    }
    mRegelaarViewUpdates.IgnoreStop(RegelaarNummer, IgnoreStop != 0);
}

void
Regelaar::ResetIgnoreStop ()
{
    if (IgnoreStop == 1)
    {
        IgnoreStop = 0;
        mRegelaarViewUpdates.IgnoreStop(RegelaarNummer, IgnoreStop != 0);
    }
}

void
Regelaar::EchteSnelheid (int Snelheid)
{
    regelCount = 0;               // we zijn in een nieuw blok gereden
    {
        sprintf (caption, "%d km/h", Snelheid);
        // I could use SetWindowText but unfortunatly
        // this can cause deadloc if you want to close the complete
        // application since it will wait until the SetWindowText message
        // is completed. Now I just post the message I don't even
        // care if that comes through or not.
        // The message handle for this regelaar will receive this message
        // and will cause the WM_ECHTE_SNELHEID call
        mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, -1);
    }
}

#define BASEX     7
#define BASEY    48
#define YLENGTE  73



void
Regelaar::GeefProgress (int Snelheid)
{
    // Snelheid ligt tussen 0 en 63

    gemetenSnelheid = Snelheid;

    // test of de trein stil staat terwijl die spanning heeft
    if ((gemetenSnelheid == 0) && (HuidigeSnelheid != 0)
            && (KortSluitCount == 0))
    {
        // ik krijg niets van de loc terug
        if (stilStandCount == 40)
        {
            // de trein staat nu 4 sec stil dus er is iets mis
            memcpy (caption, "STILSTAND", 10);
            mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, -1);
        }
        if (stilStandCount == 50)
        {
            // de trein staat nu 4 sec stil dus er is iets mis
            memcpy (caption, " ", 2);
            mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 1);
            stilStandCount = 30;
        }
        else
        {
            stilStandCount += 1;
        }
    }
    else
    {
        stilStandCount = 0;
    }

    if (lastRegelKeuze == 1)
    {
        // als de last afhankelijke regeling gebruikt wordt dan
        // wil ik de instelling van de regelaar zien
        Snelheid = (int) vorigeSnelheid;
    }
    if (Snelheid != vorigeProgresSnelheid)
    {
        mBaanMessage.Post (WM_GEEF_PROGRESS, RegelaarNummer, Snelheid, 0);
        vorigeProgresSnelheid = Snelheid;
    }
}


//Regelaar::OnPaint ()
//{
// CDialog::OnPaint ();
//  GeefProgress (HuidigeSnelheid);
//}


void
Regelaar::RegelaarStop (void)
{
    InterneSnelheid = 0;
    VerlichtingCount = 0;
    // Aangezien dit op kortsluiting lijkt gebruik ik dezefde variable
    KortSluitCount = 1000000;     // lekker lang
    vorigeSnelheid = 0;
}

void
Regelaar::RegelaarStopResume (void)
{
    InterneSnelheid = 0;
    VerlichtingCount = 0;
    // Aangezien dit op kortsluiting lijkt gebruik ik dezefde variable
    KortSluitCount = 0;           // reset
}

void
Regelaar::KortSluiting (void)
{
    InterneSnelheid = 0;
    KortSluitCount = KORTSLUIT;
    vorigeSnelheid = 0;
}

int
Regelaar::Snelheid (int Gewenst, int Alfa, int Clip)
{
    int in;
    static int meetCount;

    if (KortSluitCount)
    {
        if (KortSluitCount > KORTSLUIT)
        {
            if (KortSluitCount % 10 == 0) // de trein staat nu 4 sec stil dus er is iets mis
            {
                memcpy (caption, "STOP", 5);
                mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 0);
            }
        }
        else
        {
            memcpy (caption, "KORTSLUITING", 13);
            mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 0);
#ifdef AAP
            if (KortSluitCount % 10 == 0) // de trein staat nu 4 sec stil dus er is iets mis
            {
                memcpy (caption, "KORTSLUITING", 13);
                mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 0);
            }
            if (KortSluitCount % 10 == 5) // de trein staat nu 4 sec stil dus er is iets mis
            {
                memcpy (caption, " ", 2);
                mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 0);
            }
#endif
        }

        KortSluitCount -= 1;
        return 0;
    }
    in = (Gewenst << SNELHEID_SHIFT);
    in = in - InterneSnelheid;
    if (in < 0)
        in = (in * Alfa - 32767) >> 15;
    else
        in = (in * Alfa + 32767) >> 15;

    if (in > Clip)
    {
        in = Clip;
    }
    if (in < -Clip)
    {
        in = -Clip;
    }
    in += InterneSnelheid;
    if (in < 0)
        in = 0;

    InterneSnelheid = in;
    in = in >> SNELHEID_SHIFT;
    // 1 en 2 zijn richting commandos
    if (in < 3)
        in = 0;

    HuidigeSnelheid = in;

    // goed nu heb ik de huidigeSnelheid.
    // Alleen als er een alpha is en we zitten
    // niet in een stop toestand dan gaan we de lastafhankelijke
    // regeling doen
    switch (lastRegelKeuze)
    {
    case 1:

        // zitten we al wat langer in dit blok?
        // Als we namelijk net in het blok gereden zijn is de
        // AD convereter nog niet stabiel.
        if ((regelCount >= REGEL_COUNT_MAX) && (Gewenst > 2))
        {
            int min, max;
            int xk;
            float yk;
            float ek;
            float lpf;
            float pp, pi, pd;

            xk = (int) (gemetenSnelheid * (gainHelling * Gewenst + gainOffset));
            ek = HuidigeSnelheid - xk;
            yk = vorigeSnelheid;

            //---------------------------------------------------------------
            // Calculate Lowpass Filter for D-term: use x[k] instead of e[k]!
            //---------------------------------------------------------------
            lpf = c1 * lpf1 + c2 * (xk + xk1);


            //-----------------------------------------------------------
            // Calculate PID controller:
            // y[k] = y[k-1] - Kc*(PV[k] - PV[k-1] +
            //                     Ts*e[k]/Ti -
            //                     Td/Ts*(lpf[k] - 2*lpf[k-1]+lpf[k-2]))
            //-----------------------------------------------------------
            pp = -kc * (xk - xk1);        // y[k] = y[k-1] - Kc*(PVk - PVk-1)
            pi = k0 * ek;         //      + Kc*Ts/Ti * e[k]
            pd = -k1 * (lpf - 2.0 * lpf1 + lpf2);
            yk += pp + pi + pd;

            vorigeSnelheid = yk;

            xk1 = xk;             // PV[k-1] = PV[k]
            lpf2 = lpf1;          // update stores for LPF
            lpf1 = lpf;


            min = 0;              // de regeling max kleiner gaan ivm helling
            if (min < (HuidigeSnelheid - plusMinus))
            {
                min = HuidigeSnelheid - plusMinus;
            }

            if (vorigeSnelheid < min)
            {
                vorigeSnelheid = (float) (HuidigeSnelheid - plusMinus);
            }


            max = MaxSnelheid;
            if (max > (HuidigeSnelheid + plusMinus))
            {
                max = HuidigeSnelheid + plusMinus;
            }

            if (vorigeSnelheid > max)
            {
                vorigeSnelheid = (float) (max);
            }
            in = (int) vorigeSnelheid;
        }
        else
        {
            if (Gewenst > 2)
            {
                // nog tekort in dit blok dus geen regeling
                in = (int) vorigeSnelheid;
            }
            else
            {
                // geen regeling meer want we gaan naar 0
                vorigeSnelheid = in;
            }
        }
        if (in < 3)
            in = 0;
        break;
    case 2:
    case 3:
    {
        static int totalGemeten;
        if ((Gewenst == HuidigeSnelheid) && (Gewenst > 2))
        {
            if (meetCount <= 0)
            {
                meetCount = 0;
                totalGemeten = 0;
            }
            if (regelCount >= REGEL_COUNT_MAX)
            {
                // alleen meten als we lang genoeg in het blok zitten
                meetCount += 1;
                if (meetCount > 100)
                {
                    // start van de meeting
                    totalGemeten += gemetenSnelheid;
                }
                if (meetCount > 200)
                {
                    // na 1000 counts (10 sec) gaan we de gain uitrekenen
                    if (lastRegelKeuze == 2)
                    {
                        lastStand1 = Gewenst;
                        if (lastStand1 >= lastStand2)
                        {
                            lastStand2 = lastStand1 + 1;
                        }
                        lastGain1 = lastStand1 * 100.0 / totalGemeten;
                    }
                    else
                    {
                        lastStand2 = Gewenst;
                        if (lastStand1 >= lastStand2)
                        {
                            lastStand1 = lastStand2 - 1;
                        }
                        lastGain2 = lastStand2 * 100.0 / totalGemeten;
                    }
                    // klaar zet de lastafhankelijke regeling aan
                    // stop de trein
                    initPid ();
                    zetGewensteSnelheid (0);
                    lastRegelKeuze = 1;
                    meetCount = 0;
                }
            }
        }
        else
        {
            meetCount = 0;
        }
    }
        break;
    case 4:
    {
        static int meetArray[100];
        GewensteSnelheid = 50;
        if ((meetCount <= 0) || (meetCount > 100))
        {
            meetCount = 0;
            Verandering = 1;
        }
        meetArray[meetCount] = gemetenSnelheid;
        meetCount += 1;
        if (meetCount == 100)
        {
            int x, max;

            zetGewensteSnelheid (0);
            lastRegelKeuze = 1;
            meetCount = 0;
            // zoek de dode tijd
            for (x = 0; x < 100; x++)
            {
                if (meetArray[x] > 3)
                {
                    break;
                }
                dodeTijd = x;
            }
            // zoek het maximum
            max = 0;
            for (x = 0; x < 100; x++)
            {
                //          printf("%d %d\n",x,meetArray[x]);
                if (meetArray[x] > max)
                {
                    max = meetArray[x];
                }
            }
            max = (int) (0.5 * max);
            // zoek het eerste sample wat groter is dan het nieuwe
            // maximium
            for (x = (int) dodeTijd; x < 100; x++)
            {
                if (meetArray[x] >= max)
                {
                    // nieuwe helling berekening
                    helling =
                            (float) meetArray[x] * (gainHelling * 50 +
                                                    gainOffset) / (x - dodeTijd);
                    dodeTijd += 1;
                    break;
                }
            }
            initPid ();

        }

    }

    }

#ifdef MEETINGUIT
    vorigeSnelheid = (float) in;
    if (alpha == 0)
    {
        GewensteSnelheid = 50;
        if (count == 0)
        {
            file = fopen ("out.txt", "w");
            Verandering = 1;
        }
        count += 1;
        if (count == 100)
        {
            alpha = 0.1;
            count = 0;
            fclose (file);
            Gewenst = 0;
        }
        fprintf (file, "%d\n", gemetenSnelheid);
    }
#endif
    regelCount += 1;
    return in;
}

// dit wordt alleen voor de snelhied instelling van de achterliggende
// blokken gebruikt. 
int
Regelaar::achterLiggendeBlokken (int snelheid)
{
    int minSnelheid = 0;

    if (KortSluitCount)
    {
        // kortsluiting setting of stop indicatie
        return 0;
    }
    // als er een snelheid instelling is dan minimaal
    // de MinSnelheid op de achterliggende blokken
    if (GewensteSnelheid > 0)
    {
        if (snelheid > MinSnelheid)
            minSnelheid = snelheid;
        else
            minSnelheid = MinSnelheid;
    }
    return minSnelheid;
}


int
Regelaar::GeefStopSnelheid ()
{
    int nSnelheid;


    if (IgnoreStop)
        return GeefSnelheid (100);

    Verandering = 0;
    nSnelheid = Snelheid (0, StopAlfa, StopClip);

    return nSnelheid;
}

#define HARD_REM_DREMPEL 5

int
Regelaar::GeefSnelheid (int SnelheidLimit)
{
    int nSnelheid, Gewenst;


    if (Verandering)
    {
        Verandering = 0;
        InterneSnelheid = GewensteSnelheid << SNELHEID_SHIFT;     /* meteen effect */
    }
    Gewenst = GewensteSnelheid;
    if (Gewenst > SnelheidLimit)
        Gewenst = SnelheidLimit;
    if ((Gewenst + HARD_REM_DREMPEL) < HuidigeSnelheid)
        return Snelheid (Gewenst, StopAlfa, StopClip);

    /* als de gewenste instelling kleiner dan minimum neem die dan */
    if (Gewenst < MinSnelheid)
        return Snelheid (Gewenst, NormaalAlfa, NormaalClip);


    if (Gewenst > MaxSnelheid)
        Gewenst = MaxSnelheid;
    if (Gewenst < MinSnelheid)
        Gewenst = MinSnelheid;
    nSnelheid = Snelheid (Gewenst, NormaalAlfa, NormaalClip);
    return nSnelheid;
}

int
Regelaar::GeefRemSnelheid (int remweg)
{
    int i;

    i = 0;
    if (remweg < MIN_LENGTE)
    {
        return 0;
    }
    while ((i < MaxSnelheid) && (RemWegLengte[i] < remweg))
    {
        i++;
    }
    return i;
}


// wordt aangeroepen via de message thread om de caption te updaten
int
Regelaar::OnEchteSnelheid ()
{
    mTd.tdStart (td);
    mRegelaarViewUpdates.Meldingen(RegelaarNummer, caption);
    mTd.tdStop (td);
    return 0;
}

void
Regelaar::LaatsteWagonVuil ()
{
    memcpy (caption, "VUIL", 5);
    mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 0);
}

// De trein is tever al dus ik neem aan dat de laastse wagon
// verloren is
void
Regelaar::LaatsteWagonStaatStil ()
{
    // Noodstop we zijn de laatste wagon kwijt
    if (GewensteSnelheid != 0)
    {
        //      m_Snelheid.SetPos (MaxSnelheid);
    }
    GewensteSnelheid = 0;
    Verandering = 1;
    memcpy (caption, "Verloren", 9);
    mBaanMessage.Post (WM_ECHTE_SNELHEID, RegelaarNummer, 0, 0);
}


int
Regelaar::getGewensteSnelheid ()
{
    if (GewensteSnelheid < 3)
    {
        // 0 is uit
        // 1 en 2 zijn vooruit terug commands
        return 0;
    }
    return GewensteSnelheid;
}


/*------------------------------------------------------------------
  Purpose : This function initialises the Allen Bradley Type A PID
             controller.
  Variables: p: pointer to struct containing all PID parameters
                   Kc.Ts
             k0 =  -----   (for I-term)
                    Ti
                       Td
             k1 = Kc . --  (for D-term)
                       Ts
             The LPF parameters are also initialised here:
             lpf[k] = lpf1 * lpf[k-1] + lpf2 * lpf[k-2]
  Returns : No values are returned
  ------------------------------------------------------------------*/
void
Regelaar::initPid ()
{
    // Input
    // ts sampling time
    // ti integration timeconstant
    // kc propotional gain
    // td derivative gain
    // k_lpf lowpass filter for differental term
    t_s = 0.1;
    kc = 1.2 / (dodeTijd * t_s * helling / t_s);
    t_d = 0.5 * dodeTijd * t_s;
    t_i = 2 * dodeTijd * t_s;

    // init memories
    lpf1 = lpf2 = 0;
    xk1 = 0;
    if (t_i == 0.0)
    {
        k0 = 0.0;
    }
    else
    {
        k0 = kc * t_s / t_i;
    }
    k1 = kc * t_d / t_s;
    c1 = (2.0 * k_lpf - t_s) / (2.0 * k_lpf + t_s);
    c2 = t_s / (2.0 * k_lpf + t_s);

    // bereken de gain curve
    gainHelling = (lastGain2 - lastGain1) / (lastStand2 - lastStand1);
    gainOffset = lastGain1 - gainHelling * lastStand1;
}                               // init_pid3()
