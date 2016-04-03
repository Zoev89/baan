// BaanWT.h: interface for the BaanWorkerThreadInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_BAANWT_H_)
#define _BAANWT_H_

#include "baanTypes.h"
#include "IMessage.h"
#include "IBaanWT.h"
#include "IBaanMessage.h"
#include "IWissels.h"
#include <boost/thread.hpp>
#include "ITd.h"

#define BLOK_VRIJ           0x00	// Vrij te gebruiken
#define BLOK_VOORUIT        0x05 
#define BLOK_ACHTERUIT      0x06        // BLOK_ACHTERUIT = BLOK_VOORUIT +1 hou dit
#define BLOK_VOORUITCHECK   0x07	// Het blok waar de trein in gaat komen
#define BLOK_ACHTERUITCHECK 0x08	// Het blok waar de trein in gaat komen
#define BLOK_STOP           0xf0	// Gebruikt voor eind blokken
#define BLOK_BEZET_MASK     0x0f

#define HARDWARE_MIN_ADRES    1
#define HARDWARE_MAX_ADRES 1023

#define HARDWARE_IO_LSB_BITNUMMER 0
#define HARDWARE_IO_MSB_BITNUMMER 15

#define HW_POS   1		// default gebruik de pInfo->vooruit/achteruit voor de actuele waarde
#define HW_NEG   2
#define HW_SEIN_GROEN 61
#define HW_SEIN_GEEL  62
#define HW_SEIN_ROOD  63

#define IO_COMMAND0 0
#define IO_COMMAND1 (1<<4)
#define IO_COMMAND2 (2<<4)
#define IO_COMMAND3 (3<<4)


#define MAX_SNELHEID 1000


#define ONTKOPPELAAR     1
#define STANDAARD_WISSEL 2
#define KRUISING         3
#define ENGELSE_WISSEL   4
#define DRIEWEG_WISSEL   5
#define LAMP             6
#define DRAAISCHIJF      7
#define LAATSTE_ENTRY    8  // wordt gebruikt de blk file te saven.

#define DOT_SIZE          6	// grootte van de dot van een bezet blok
#define KLEINE_DOT_SIZE   4	// grootte van de dot van een check blok
#define RESET_SIZE 16


class BaanWT: public IBaanWT
{
public:
    BaanWT(IMessage& msg, IBaanMessage& baanMessage, IWissels& wissels, ITd& td, BaanInfo_t *baanInfo);
    // IBaanWT
    virtual void InitWorkThread (void) override;
    virtual void StartWorker(void) override;
    virtual void StopWorker(void) override ;
    virtual void ericSleep (int tijdInMs) override;
    virtual void BaanCheckLengte (int RegelNummer, int NeemKopBlokMee) override;
private:
    void GeefBlokVrij (BlokPointer_t * pBlok);
    void BaanSimulatie (void);
    void BaanWorkerThreadProc (void);
    void RegelAanvraag (void);
    int localZendUsbBuffer (unsigned char *buffer,
                        unsigned char *bufferOnt, int lengte, int *stopBit);
    void geefIdleBytes (int *byte1, int *byte2);
    int BaanAansturing (hardwareCom_t * huidigeArray,
                    hardwareCom_t * vorigeArray);
    void initSimulatieArray (void);
    void StartHardware (void);
    void StopHardware (void);
    void ResetHardware (void);
    void SetSnelheid (int Regelaar, int Snelheid);
    void BaanGeefVrij (int BlokNummer);
    void zetSein (BlokPointer_t * pBlok, hardwareArray_t * array,
             SeinState_t sein);
    void prepareSein (BlokPointer_t * pBlok, SeinState_t sein);
    void BaanSetSein (void);
    int BaanTestBelegCheckblok (int regelaarNummer);
    void BaanSnelheidControl (void);
    void BaanBehandelIO (hardwareCom_t * array);
    void BaanDetection (hardwareCom_t * array);



    IMessage& mMessage;
    IBaanMessage& mBaanMessage;
    IWissels& mWissels;
    ITd& mTd;

    BaanInfo_t* pInfo;
    boost::thread *workerThread;

    int tdaanStuur;
    int tdwt;
};


class BaanInfo
{
public:
    BaanInfo(BaanInfo_t * baanInfo);
    
    virtual BlokPointer_t * GetBlokPointer(int blokNummer);
    virtual BaanZoekResult NextBlok(BlokPointer_t *input, int richting);
    std::vector<int> & GetBlokList(int wisselNummer, int knoopPuntNummer);
    BaanInfo_t *GetBaanInfo() { return m_baanInfo;};
private:
    BaanInfo_t * m_baanInfo;
};

#endif // !defined(_BAANWT_H_)
