#if !defined(_REGELAAR_H)
#define _REGELAAR_H

#include "IRegelaarView.h"
#include "IRegelaarViewUpdates.h"
#include "IBaanTreinen.h"
#include "IBaanDoc.h"
#include "IRegelaarInstellingenDialoog.h"
#include "IBaanMessage.h"
#include "ITelefoonConnectie.h"
#include "ITd.h"

// Regelaar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Regelaar dialog

#define LOCSOORT_SIZE 20

#define PID

class Regelaar : public IRegelaarView
{
// Construction
public:
  Regelaar (IRegelaarViewUpdates& regelaarViewUpdates, IBaanTreinen& baanTreinen, IBaanDoc& baanDoc, IRegelaarInstellingenDialoog &regelDialoog, IBaanMessage& baanMessage,
            ITelefoonConnectie& telefoonConnectie, ITd& td);			// standard constructor

  // IRegelaarView
  virtual void SetSnelheid(int snelheid);
  virtual int GetSnelheid();
  virtual void SetRichting(int vt);
  virtual int GetRichting();
  virtual void SetIgnoreStop(bool ignore);
  virtual bool GetIgnoreStop();
  virtual void RunProgramClicked(bool run);
  virtual void VerwijderClicked();
  virtual void PropertiesClicked();
  virtual std::string GetBitmapFileName();


  void Show(bool show);

  int RegelaarNummer;
  RegelaarAanvraag_t *m_pRegelAanvraag;
  char locNaam[20];
  char locSoort[LOCSOORT_SIZE];
  int td;			// timedocter debuging

  char caption[20];
  int Gebruikt;

  /*
   ** Maximum size of the current bitmap for the button is
   ** 38x18 pixels
   **/
  std::string bitmapName;


  BlokPointer_t *pKopBlok;
  int Richting;			/* 0 is vooruit 1 is terug */
  int Lengte;

  int MaxSnelheid;
  int MinSnelheid;

  int IgnoreStop;
  int ELoc;			// 1 = eloc else 0 
  int programRunning;

  // Voor de uitvoering van een programma kan je hier per
  // locomotief zijn langzaan en rijden snelheid geven.
  int Langzaam, Rijden;


  /* De snelheids curves zijn relative curves tussen 0 en 1 */
  float fStopAlfa;
  float fNormaalAlfa;
  int StopAlfa, StopClip;
  int NormaalAlfa, NormaalClip;

  void GeefProgress (int Snelheid);
  void KortSluiting (void);
  int GeefRemSnelheid (int remweg);

  int GeefSnelheid (int MaxSnelheid);
  int GeefStopSnelheid ();
  void RegelaarStop ();		// doe een noodstop
  void RegelaarStopResume (void);
  void InitRemWeg (float Snelheid1, float Afstand1, float Snelheid2,
		   float Afstand2);
  int zetGewensteSnelheid (int snelheid);

  void Langzamer ();
  void Sneller ();
  void HandRegelaarStop ();
  void RichtingVerandering ();
  void WisselIgnoreStop ();
  void ResetIgnoreStop ();
  void EchteSnelheid (int Snelheid);	// displayed de snelheid in Km/h
  int getGewensteSnelheid ();

  void CleanUp (void);

  int InterneSnelheid;		/* intern opgeschaald */

  int TotaalAfstand;
  int TotaalTicken;
  int TopSnelheid;		/* de snelheid waarbij de regelaar op het maximum staat in 63/TopSnelheidKmh */
  int TopSnelheidKmh;
  unsigned int tijd;		// tijd van start van de echte snelheid meeting
  int blokLengte; 		// De meeting kan over meerder blokken plaats vinden

  char RegelaarFileName[300];	/* zodat als we de het programma weer aanzetten we de status hebben */
  char programmaNaam[200];	// wordt gebuikt om een nieuw programma te kunnen laden


  // voor het niet altijd checken van het check blok is hier
  // een decimatie variabele. Het gebruik van de decimatie is afhankelijk van de lengte 
  // van het blok.
  int decimatie;

  // variabelen voor de last afhankelijk regeling
  int lastRegelKeuze;
  float dodeTijd,helling;
  float k_lpf;
  float lastGain1,lastGain2;	// de gain van de gemeten snelheid naar de regelaar snelheid
  int   lastStand1,lastStand2;  // stand van de regelaar waarbij de meeting is uitgevoerd
  int plusMinus;		// de ruimte waarin de regling moet blijven
  void initPid();

  int laatsteWagonCheck;
  int herlaadProgramma;		// geeft aan of het programma geherladen moet worden
  int laatsteWagonDecimatie;
  int laatsteWagonCheckError;	// Het aantal random fouten. Dit kan gebruikt worden
  // om te zien of de as vuil wordt
  int laatsteWagonCheckTotal;	// voor het percentage errors uit te kunnen rekenen
  int laatsteWagonDetect;

  int achterLiggendeBlokken (int snelheid);
  void LaatsteWagonVuil ();
  void LaatsteWagonStaatStil ();

  int aantalFouten[MAX_NOBLOKS];


  void OnInitDialog (int regelaar);
  void OnRegelProperties ();
  int OnEchteSnelheid ();
  void SnelheidProgress(int snelheid);
// Implementation
protected:


private:
  IRegelaarViewUpdates& mRegelaarViewUpdates;
  IBaanTreinen& mBaanTreinen;
  IBaanDoc& mBaanDoc;
  IRegelaarInstellingenDialoog &mRegelInstellingenDialoog;
  IBaanMessage& mBaanMessage;
  ITelefoonConnectie& mTelefoonConnectie;
  ITd& mTd;

  /* Snelheid gebruikt in de hardware word alleen voor de slider
   ** bar gebruikt
   */
  int HuidigeSnelheid;		// geeft de huidigesnelheid vn de regelaar weer in hardware eenheden
  int GewensteSnelheid;		/* in hardware eenheden */
  int vorigeProgresSnelheid;    // we onthouden de snelheid die naar de progress bar gaat om
                                // events te verminderen

  int Verandering;		/* Als de regelaar bewogen is dan wordt deze gezet */

  int RemWegLengte[64];


  int Snelheid (int Gewenst, int Alfa, int Clip);

  /* variablen voor het uitrekenen van de remweg */
  int mStand1, mStand2;
  int mAfstand1, mAfstand2;
  int KortSluitCount;
  int VerlichtingCount;

  // variabelen voor de last afhankelijke regeling
  int gemetenSnelheid;
  float vorigeSnelheid;
  int regelCount;		// voor het tellen hoelang we al in dit blok zijn

  // voor stilstand detectie
  int stilStandCount;

  float gainHelling,gainOffset;
  float t_s;
  float t_i;
  float kc;
  float t_d;
  float k0,k1;
  float lpf1,lpf2;
  float c1,c2;
  float xk1;
};

#endif // !defined(_REGELAAR_H)
