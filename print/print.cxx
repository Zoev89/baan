#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "leesdata.h"

#include "usbinout.h"



#define AT90S2333 1
#define AT90S4433 2
#define ATMEGA8   3
char deviceString[][10] =
{
    "unknown",
    "2333",
    "4333",
    "m8",
};

#define AANTAL_IO 16            // aantal io pins per io controller

#define POS_SPANNING  60
#define NEG_SPANNING -60

#define MAX_LENGTH  200         // maximum lengte van een string tijdens het programmeren
#define MAX_BLOK_ADRES (1022-5) // het laatste blok adres
#define MAX_IO_ADRES   1022     // het laatste io adres

char blkFile[MAX_LENGTH];
char blokFile[MAX_LENGTH];
char ioFile[MAX_LENGTH];

void programPrepare (int *padres, int *ptype, int *pdevice);



void
adresDataTest ()
{
    int cx, stop;
    int adres, data, ad;
    unsigned char bufferOut[64], bufferIn[64];

    if ((cx = openUsbPort ()))
    {
        printf ("usbPort niet gevonden return status %d\n", cx);
        return;
    }

    printf ("Geef adres en data\n");
    printf ("positief is 1 en negatief is 2\n");
    printf ("61 is groen\n");
    printf ("62 is geel\n");
    printf ("63 is rood\n");
    printf ("geef <=0 om terug tekeren naar het hoofd menu\n");
    do
    {
        scanf ("%d", &adres);
        if (adres > 0)
        {
            if ((adres < 1) || (adres > MAX_BLOK_ADRES))
            {
                printf ("Invalid adres [1..%d]\n", MAX_BLOK_ADRES);
                continue;
            }
            scanf ("%d", &data);
            if ((data < 0) || (data > 63))
            {
                printf ("Invalid data [0..63]\n");
                continue;
            }
            data = (data << 2) + (adres >> 8);
            ad = adres & 0xff;

            // verzend de data
            bufferOut[0] = ad;
            bufferOut[1] = data;
            cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
            if (cx < 0)
            {
                printf ("error in usb communicatie\n");
                continue;
            }

            // zend een dummy commando om de return status te zien
            // verzend de data
            bufferOut[0] = 0xff;
            bufferOut[1] = 0xff;
            cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
            if (cx != 1)
            {
                printf ("Blok op adres %d geeft geen response!\n", adres);
            }
            else
            {
                printf ("Return status commando 0x%x of %d\n", bufferIn[0],
                        bufferIn[0]);
            }
        }
    }
    while (adres > 0);
    closeUsbPort ();
}
void tijdelijkeTest()
{
    int cx, stop;
    int adres, data, ad;
    unsigned char bufferOut[256], bufferIn[256];

    if ((cx = openUsbPort ()))
    {
        printf ("usbPort niet gevonden return status %d\n", cx);
        return;
    }

    printf ("Geef adres en data\n");
    scanf("%d",&adres);
    data = 1;
    data = (data << 2) + (adres >> 8);
    ad = adres & 0xff;

    // verzend de data
    bufferOut[0] = ad;
    bufferOut[1] = data;
    cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
    // zend een dummy commando om de return status te zien
    // verzend de data
    bufferOut[0] = 0xff;
    bufferOut[1] = 0xff;
    cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
    if (cx != 1)
    {
        printf ("Blok op adres %d geeft geen response!\n", adres);
        closeUsbPort ();
        return;
    }
    else
    {
        printf ("Return status commando 0x%x of %d\n", bufferIn[0],
                bufferIn[0]);
    }
    for (int i=0; i<256; i+=2)
    {
        data = 31;
        data = (data << 2) + (adres >> 8);
        ad = adres & 0xff;
        // verzend de data
        bufferOut[i+0] = ad;
        bufferOut[i+1] = data;

    }
    int count=0;
    int errorcount=0;
    int byteCount = 0;
    int transmitAantal = 128;
    do
    {

        cx = zendUsbBuffer (bufferOut, bufferIn, transmitAantal, &stop);
        count += 1;
        if (cx != (transmitAantal/2))
        {
            errorcount++;
            printf("Blok op adres %d geeft geen response aantal %d count=%d %%=%g!\n", adres,cx,count,(float)errorcount/(float)count);
        }
        else
        {
//          printf ("Return status commando 0x%x of %d\n", bufferIn[0],
//                  bufferIn[0]);
        }
        for(int i=0; i<cx; ++i)
        {
            if (bufferIn[i] != byteCount)
            {
                printf("At count=%d Byte at %d not in sync 0x%x expect 0x%x\n",count,i,bufferIn[i],byteCount);
                byteCount = bufferIn[i];
            }
            byteCount = (byteCount+1) & 0xff;
        }
    }
    while (adres > 0);

}

void
adresIODataTest ()
{
    int cx;
    int adres, data, ad, subAdres;
    int stop;
    unsigned char bufferOut[64], bufferIn[64];

    if ((cx = openUsbPort ()))
    {
        printf ("usbPort niet gevonden return status %d\n", cx);
        return;
    }

    printf ("Geef adres subadres en data\n");
    printf ("Wissels data=0 is rechtdoor en data=3 is buigend\n");
    printf ("geef <=0 om terug tekeren naar het hoofd menu\n");
    do
    {
        scanf ("%d", &adres);
        if (adres > 0)
        {
            if ((adres < 1) || (adres > MAX_BLOK_ADRES))
            {
                printf ("Invalid adres [1..%d]\n", MAX_BLOK_ADRES);
                continue;
            }
            scanf ("%d", &subAdres);
            if ((subAdres < 0) || (subAdres > 15))
            {
                printf ("Invalid subadres [0..15]\n");
                continue;
            }

            scanf ("%d", &data);
            if ((data < 0) || (data > 3))
            {
                printf ("Invalid data [0..3]\n");
                continue;
            }
            data = (data << 6) + (subAdres << 2) + (adres >> 8);
            ad = adres & 0xff;

            // verzend de data
            bufferOut[0] = ad;
            bufferOut[1] = data;
            cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
            if (cx < 0)
            {
                printf ("error in usb communicatie\n");
                continue;
            }


            // zend een dummy commando om de return status te zien
            bufferOut[0] = 0xff;
            bufferOut[1] = 0xff;
            cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
            if (cx != 1)
            {
                printf ("Blok op adres %d geeft geen response!\n", adres);
            }
            else
            {
                printf ("Return status commando %x\n", bufferIn[0]);
            }
        }
    }
    while (adres > 0);

    closeUsbPort ();

}

// verzend 1 OI controller commando retuneer de reactie van de controller
int zendIOData(int adres,int subAdres,int data)
{
    int stop; // return waarde van de stop paniek knop
    int cx;
    unsigned char bufferOut[64], bufferIn[64];

    data = (data << 6) + (subAdres << 2) + (adres >> 8);
    int ad = adres & 0xff;

    // verzend de data
    bufferOut[0] = ad;
    bufferOut[1] = data;
    cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
    if (cx < 0)
    {
        printf ("error in usb communicatie\n");
        return -1;
    }

    // zend een dummy commando om de return status te zien
    bufferOut[0] = 0xff;
    bufferOut[1] = 0xff;
    cx = zendUsbBuffer (bufferOut, bufferIn, 2, &stop);
    if (cx != 1)
    {
        printf ("Blok op adres %d geeft geen response!\n", adres);
        return -2;
    }
    // retuneer de response van de controller
    return bufferIn[0];
}

//
// zend 10x alle bitten aan en controlleer met de 2 ledjes of die even 3 maal knipperen.
// 
void
wisselPrintTest ()
{
    int cx;
    int adres, subAdres;

    if ((cx = openUsbPort ()))
    {
        printf ("usbPort niet gevonden return status %d\n", cx);
        return;
    }

    printf ("Geef adres van de IO controller op het wissel bord\n");
    scanf ("%d", &adres);

    if ((adres >= 1) && (adres < MAX_BLOK_ADRES))
    {
        for (int repeat = 0; repeat<10;repeat++)
        {
        for (subAdres = 0; subAdres < 16; subAdres += 1)
        {
            printf("subAdres %d\n",subAdres);
            zendIOData(adres,subAdres,0);
            zendIOData(adres,subAdres,0);
            zendIOData(adres,subAdres,0);
            zendIOData(adres,subAdres,3);
            zendIOData(adres,subAdres,3);
            zendIOData(adres,subAdres,3);
        }
        }
    }
    else
    {
        printf ("Invalid adres [1..%d]\n", MAX_BLOK_ADRES);
    }


    closeUsbPort ();

}



void
wacht (int millisec)
{
    long end;
    end = (long) ((1.0 / CLOCKS_PER_SEC + millisec / 1000.0) * CLOCKS_PER_SEC);
    end += clock ();
    while (end > clock ());

}


void
blokAdresData (int adres, int data, int testBlok, int *ret)
{
    int cx, i, byte1, byte2, stop;
    unsigned char bufferOut[64], bufferIn[64];

    for (i = 0; i < 6; i++)
    {
        byte1 = (adres + i) & 0xff;
        byte2 = (adres + i) >> 8;
        if ((testBlok == -1) || (i == testBlok))
        {
            // voor dit blok deze data
            byte2 |= data << 2;
        }
        bufferOut[2 * i] = byte1;
        bufferOut[2 * i + 1] = byte2;
    }

    cx = zendUsbBuffer (bufferOut, bufferIn, 2 * 6, &stop);
    for (i = 0; i < 6; i++)
    {
        if (i < cx)
            ret[i] = bufferIn[i];
        else
            ret[i] = -1;
    }
}



void
programBlokIOPrint ()
{
    char text[MAX_LENGTH];
    int adres, type;
    int device = 0;

    programPrepare (&adres, &type, &device);
    if (adres == 0)
        return;

    if (type == 1)
    {
        // programmeer een blok printje
        printf ("\n\nGeeft base adres [1..%d] default=%d\n", MAX_BLOK_ADRES,
                adres);
        fflush (stdin);
        fgets (text, MAX_LENGTH, stdin);
        if (text[0] != 0)
        {
            sscanf (text, "%d", &adres);
        }
        if (adres < 1)
        {
            printf ("Adres mag niet kleiner zijn dan 1\n");
            return;
        }
        if (adres > MAX_BLOK_ADRES)
        {
            printf ("Adres mag niet groter zijn dan %d\n", MAX_BLOK_ADRES);
            return;
        }
        // programmeer het device
        if (device == ATMEGA8)
        {
            // notes voor de fuses
            // 00011111 are the fuse bits read from an ATmega8
            // 0xxxxxxx - BODLEVEL 4V (default 2.7V)
            // x0xxxxxx - brownout detection enabled
            // xxSUxxxx - reset delay, datasheet p28
            // xxxxCKSE - clock select, datasheet p24
            sprintf (text,
                     "avrdude -P usb -c avrispmkii -p %s -B8 -U flash:w:%s8.hex:i -U eeprom:w:%s8_eeprom.hex:i "
                     "-U eeprom:w:0x%x,0x%x:m -U lfuse:w:0b00011111:m",
                     deviceString[device],
                     blokFile, blokFile, adres & 0xff, adres >> 8);
            // note de CKOPT wordt niet geprogrammerd (in de H fuses)
            // default staat die aan
        }
        else
        {
            // notes voor de fuses
            // 000010 are the fuse bits read from an AT90S2333
            // 0xxxxx - serial programming enabled
            // x0xxxx - BODLEVEL 4V (default 2.7V)
            // xx0xxx - brownout detection enabled
            // xxxCKS - reset delay select (datasheet p20)
            sprintf (text,
                     "avrdude -P usb -c avrispmkii -p %s -B8 -U flash:w:%s.hex:i -U eeprom:w:%s_eeprom.hex:i "
                     "-U eeprom:w:0x%x,0x%x:m -U fuse:w:0b00000010:m",
                     deviceString[device],
                     blokFile, blokFile, adres & 0xff, adres >> 8);
        }
        system (text);
    }
    else
    {
        // program IO print
        printf ("\n\nGeeft base adres [1..%d] default=%d\n", MAX_IO_ADRES,
                adres);
        fflush (stdin);
        fgets (text, MAX_LENGTH, stdin);
        if (text[0] != 0)
        {
            sscanf (text, "%d", &adres);
        }
        if (adres < 1)
        {
            printf ("Adres mag niet kleiner zijn dan 1\n");
            return;
        }
        if (adres > MAX_IO_ADRES)
        {
            printf ("Adres mag niet groter zijn dan %d\n", MAX_IO_ADRES);
            return;
        }
        // programmeer het device
        if (device == ATMEGA8)
        {
            // notes voor de fuses
            // 00011111 are the fuse bits read from an ATmega8
            // 0xxxxxxx - BODLEVEL 4V (default 2.7V)
            // x0xxxxxx - brownout detection enabled
            // xxSUxxxx - reset delay, datasheet p28
            // xxxxCKSE - clock select, datasheet p24
            sprintf (text,
                     "avrdude -P usb -c avrispmkii -p %s -B8 -U flash:w:%s8.hex:i -U eeprom:w:%s8_eeprom.hex:i "
                     "-U eeprom:w:0x%x,0x%x:m -U lfuse:w:0b00011111:m",
                     deviceString[device],
                     ioFile, ioFile, adres & 0xff, adres >> 8);
            // note de CKOPT wordt niet geprogrammerd (in de H fuses)
            // default staat die aan
        }
        else
        {
            // notes voor de fuses
            // 000010 are the fuse bits read from an AT90S2333
            // 0xxxxx - serial programming enabled
            // x0xxxx - BODLEVEL 4V (default 2.7V)
            // xx0xxx - brownout detection enabled
            // xxxCKS - reset delay select (datasheet p20)
            sprintf (text,
                     "avrdude -P usb -c avrispmkii -p %s -B8 -U flash:w:%s.hex:i -U eeprom:w:%s_eeprom.hex:i "
                     "-U eeprom:w:0x%x,0x%x:m -U fuse:w:0b00000010:m",
                     deviceString[device],
                     ioFile, ioFile, adres & 0xff, adres >> 8);
        }
        system (text);
    }
    if (device != ATMEGA8)
    {
        printf ("\n\n");
        printf ("    NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE\n");
        printf ("Verwijder het printje kortstondig van de program poort\n");
        printf ("voordat je er verder wat mee kan doen\n");
        printf ("    END NOTE END NOTE END NOTE END NOTE END NOTE\n");
        printf ("\n\n");
    }
}


void
vulEpromData (FILE * file, int adres, int *aandrempel, int *max, int *type)
{
    int i;
    char String[MAX_LENGTH];
    int soort, ioAandrempel, ioMax, ioType, dummy1;
    float fadres;

    // vul de eeprom met defaults
    for (i = 0; i < AANTAL_IO; i++)
    {
        aandrempel[i] = 150;
        max[i] = 250;
        type[i] = 1;
    }

    while (EricFgets (String, MAX_LENGTH, file) != NULL)
    {
        if (sscanf
                (String, "%d%f%d%d%d%d", &soort, &fadres, &ioAandrempel, &ioMax,
                 &ioType, &dummy1) == 6)
        {
            int hardwareAdres, hardwareBit;

            // als we 6 items konden lezen hebben we het over een blok of een wissel
            // Als het een blok is zal er geen adres match zijn (mits de database correct is:)
            hardwareAdres = (int) (fadres + 0.5);
            hardwareBit =
                (int) ((fadres - (float) hardwareAdres) * 100.0 + 0.5);
            if ((hardwareAdres == adres) && (hardwareBit < AANTAL_IO))
            {
                // match
                switch (soort)
                {
                case 6:
                    type[hardwareBit] = ioType;
                    // TODO max en aantaldrempel even een paar defaults
                    aandrempel[hardwareBit] = ioAandrempel;
                    max[hardwareBit] = ioMax;
                    break;
                    // TODO de andere cases moeten nog en de defaults
                }
            }
        }
    }
}

// de initele handshake voor programmering van een device
// als padres = 0 dan is er iets mis
void
programPrepare (int *padres, int *ptype, int *pdevice)
{
    char text[MAX_LENGTH];
    FILE *file;
    int byte1, byte2, adres, type;
    int device = 0;

    *padres = 0;                  // make adres ongeldig


    // eerst zien wat er aan de centrale controler hangt
    // een AT90S2333 of AT90S4333 of ATmega8
    // We doen dit door de signature te lezen met een geforceerde
    // device
    // Als er niets is bestaat de out.txt niet
    remove ("out.txt");
    if (system
            ("avrdude -P usb -c avrispmkii -p 2333 -B8 -F -q -q -U signature:r:out.txt:h"))
    {
        printf ("\n\nProbleem met avrdude programma, spanning aan?\n");
        return;
    }

    file = fopen ("out.txt", "r");
    if (file == NULL)
    {
        printf ("Chip niet aanwezig power aan?\n");
        return;
    }
    device = 0;
    if (fgets (text, MAX_LENGTH, file))
    {

        // zoek de zin met signature
        // 2333  0x1e9105
        // 4433  0x1e9203
        // mega8 0x1e9307

        /// de defines zijn precies de 1 2 3
        device = text[8] - '0';
    }
    if ((device < 1) || (device > 3))
    {
        printf ("Chip niet herkend! zie out.txt\n");
        return;
    }
    fclose (file);
    printf ("\n\nDetected device %s\n", deviceString[device]);


    // lees eeprom
    sprintf (text,
             "avrdude -P usb -c avrispmkii -p %s -B8 -q -q -U eeprom:r:out.txt:d",
             deviceString[device]);
    if (system (text))
    {
        printf ("\n\nProbleem met avrdude programma, spanning aan?\n");
        return;
    }

    // eerst 2 bytes is het adres en het derde byte het type

    file = fopen ("out.txt", "r");
    fgets (text, MAX_LENGTH, file);
    sscanf (text, "%d,%d,%d", &byte1, &byte2, &type);
    adres = byte1 + 256 * byte2;
    if (adres == 0xffff)
    {
        printf
        ("\n\nWeet je zeker dat er een blok print op de centrale controller is aangesloten\n[J/N]\n");
        fflush (stdin);
        fgets (text, MAX_LENGTH, stdin);
        printf ("-%s-\n", text);
        if ((text[0] != 'J') && (text[0] != 'j'))
        {
            return;
        }
    }
    // Haal het IO byte op om te zien wat het is

    if (type == 1)
    {
        // dit is blok controller
        printf ("Blok controller detected\n");
    }
    if (type == 2)
    {
        // dit is blok controller
        printf ("IO controller detected\n");
    }
    printf ("enter 1 voor blok controller of 2 voor IO controller\n");
    fflush (stdin);
    fgets (text, MAX_LENGTH, stdin);
    if (text[0] != 0)
    {
        sscanf (text, "%d", &type);
    }

    *padres = adres;
    *ptype = type;
    *pdevice = device;
}



void
programEeprom (FILE * database)
{
    char text[MAX_LENGTH];
    FILE *file;
    int adres;
    int device, btype;


    programPrepare (&adres, &btype, &device);
    if (adres == 0)
        return;



    if (btype == 1)
    {
        // dit is blok controller
        printf ("Blok controller detected\n");
        printf ("\n\nGeeft base adres [1..%d] default=%d\n", MAX_BLOK_ADRES,
                adres);
        fflush (stdin);
        fgets (text, MAX_LENGTH, stdin);
        if (text[0] != 0)
        {
            sscanf (text, "%d", &adres);
        }
        if (adres < 1)
        {
            printf ("Adres mag niet kleiner zijn dan 1\n");
            return;
        }
        if (adres > MAX_BLOK_ADRES)
        {
            printf ("Adres mag niet groter zijn dan %d\n", MAX_BLOK_ADRES);
            return;
        }
        sprintf (text,
                 "avrdude -P usb -c avrispmkii -p %s -B8"
                 "-U eeprom:w:0x%x,0x%x,0x%x:m",
                 deviceString[device], adres & 0xff, adres >> 8, btype);
        system (text);


        printf ("\n\n");
        return;
    }
    if (btype == 2)
    {
        int aandrempel[AANTAL_IO], max[AANTAL_IO], type[AANTAL_IO];
        int i;

        while (database == NULL)
        {
            char filename[200];

            printf ("geef de filename van de baan beschrijving (.blk)\n");
            scanf ("%s", filename);
            database = fopen (filename, "r");
            if (database == NULL)
            {
                printf ("Ik kan %s niet openen\n", filename);
            }
        }

        printf ("\n\nGeeft base adres [1..%d] default=%d\n", MAX_IO_ADRES,
                adres);
        fflush (stdin);
        fgets (text, MAX_LENGTH, stdin);
        if (text[0] != 0)
        {
            sscanf (text, "%d", &adres);
        }
        if (adres < 1)
        {
            printf ("Adres mag niet kleiner zijn dan 1\n");
            return;
        }
        if (adres > MAX_IO_ADRES)
        {
            printf ("Adres mag niet groter zijn dan %d\n", MAX_IO_ADRES);
            return;
        }
        file = fopen ("out.txt", "w");
        // Het programen doen we door een eeprom text filetje te
        // maken en die dan in 1 klap te programeren.
        // Programmeer het basis adres van de eeprom en type.

        fprintf (file, "%d,%d,%d", adres & 0xff, adres >> 8, btype);

        // vul de eprom data
        vulEpromData (database, adres, aandrempel, max, type);
        for (i = 0; i < AANTAL_IO; i += 2)
        {
            fprintf (file, ",%d,%d,%d,%d,%d,%d",
                     aandrempel[i + 0],
                     max[i + 0], type[i + 0],
                     aandrempel[i + 1], max[i + 1], type[i + 1]);
        }
        fclose (file);
        remove ("out.hex");
        system ("./dec2hex out.txt out.hex");

        sprintf (text,
                 "avrdude -P usb -c avrispmkii -p %s -B8 -U eeprom:w:out.hex:i",
                 deviceString[device]);
        system (text);
        printf ("\n\n");
        return;
    }
    printf ("onbekende controller eerst programmeren\n");
}

char *spanningString(int waarde)
{
    // de waardes 255-25 en 25 zijn de default initializatie waardes van
    // spannings meeting in de controller. Als de controller nog niets gemeten
    // heeft reporteerd die deze een van deze waardes (afhankelijk van vooruit
    // of terug)
    // By the way de kortsluit drempel ligt op 70 tussen de 255-70
    static char string[10];
    if (waarde==0)
    {
        strcpy(string,"SHRT");
    }
    else if (waarde == (255-25))
    {
        strcpy(string,"+LEV");
    }
    else if (waarde == 25)
    {
        strcpy(string,"-LEV");
    }
    else
    {
        sprintf(string,"%4d",waarde-126);
    }
    return string;
}

void
blokPrintTest ()
{
    int adres, testBlok;
    int returnWaarde[6];
    int ret;
    struct spanningWaarde
    {
        int nulWaarde[2];
        int activeWaarde[2];
    } spanningStatus[6][6];

    if ((ret = openUsbPort ()))
    {
        printf ("usbPort niet gevonden return status %d\n", ret);
        return;
    }

    printf ("Geef het basis adres [1..1017]\n");
    scanf ("%d", &adres);
    if ((adres < 1) || (adres > 1017))
    {
        printf ("Invalid adres %d\n", adres);
    }
    else
    {
        int error = 0, richting;
        int t;

        for (richting = 1; richting < 3; richting++)
        {
            for (testBlok = 0; testBlok < 6; testBlok++)
            {
                // Eerst de juiste richting zetten
                blokAdresData (adres, richting, -1, returnWaarde);

                // Nu spanning op het testBlok
                blokAdresData (adres, 16, testBlok, returnWaarde);

                wacht (200);      // ik wacht 100ms extra zodat de spanning goed stabilizeerd

                // Nu weer dezelfde spanning op het testBlok 2* want ik wil niet de
                // waardes van de vorige keer hebben
                blokAdresData (adres, 16, testBlok, returnWaarde);
                blokAdresData (adres, 16, testBlok, returnWaarde);

                for (t = 0; t < 6; t++)
                {
                    spanningStatus[testBlok][t].nulWaarde[richting-1] = returnWaarde[t];
                    if ((returnWaarde[t] < 124) || (returnWaarde[t] > 129))
                    {
                        if (returnWaarde[t] == 0)
                        {
                            printf ("Kortsluiting op blok %d geraporteerd\n",
                                    t + adres);
                            error = 1;
                        }
                        else
                        {
                            printf
                            ("Incorrecte nul spanning op blok %d waarde %d met spanning op blok %d\n",
                             t + adres, returnWaarde[t], testBlok + adres);
                            error = 1;
                        }
                    }
                }
                // Eerst de richting en dan weer spanning erop dan heb ik
                // antwoorden terug van het richting commando
                blokAdresData (adres, richting, -1, returnWaarde);
                blokAdresData (adres, 16, testBlok, returnWaarde);
                for (t = 0; t < 6; t++)
                {
                    spanningStatus[testBlok][t].activeWaarde[richting-1] = returnWaarde[t];
                    if (returnWaarde[t] == 0)
                        continue;   // hebben we al getest op kortsluiting

                    returnWaarde[t] -= 126;
                    if (richting == 1)
                    {
                        if (returnWaarde[t] < POS_SPANNING)
                        {
                            printf
                            ("te lage spanning %d op blok %d drempel %d\n",
                             returnWaarde[t], t + adres, POS_SPANNING);
                            error = 1;
                        }
                    }
                    else
                    {
                        if (returnWaarde[t] > NEG_SPANNING)
                        {
                            printf
                            ("te lage spanning %d op blok %d drempel %d\n",
                             returnWaarde[t], t + adres, NEG_SPANNING);
                            error = 1;
                        }
                    }
                }
            }
        }
        if (error == 0)
        {
            printf ("Blok kaart OK\n");
        }
        else
        {
            printf ("Blok kaart FOUT\n");
            for (testBlok=0; testBlok<6; testBlok++)
            {
                for(richting=0; richting<2; richting++)
                {
                    printf("\n%d nul ",testBlok);
                    for (t=0; t<6; t++)
                    {
                        printf("%s ",spanningString(spanningStatus[testBlok][t].nulWaarde[richting]));
                    }
                }
                for(richting=0; richting<2; richting++)
                {
                    printf("\n%d act ",testBlok);
                    for (t=0; t<6; t++)
                    {
                        printf("%s ",spanningString(spanningStatus[testBlok][t].activeWaarde[richting]));
                    }
                }
            }
            printf("\n\n");
        }
    }
    closeUsbPort ();
}

int
main ()
{
    FILE *file;
    char temp[MAX_LENGTH];

    // eerst de default waarden lezen van de ini file
    file = fopen ("print.ini", "r");
    if (NULL == file)
    {
        file = fopen ("print.ini", "w");
        printf ("Generating default print.ini file\n");
        printf ("Hiermee kan je alle program files configuren\n");
        printf ("breek het programma af als de defaults niet goed zijn\n");
        printf ("de .hex suffix mag er niet aanhangen.\n");
        printf ("Het programma doet die toevoegen met\n");
        printf (".hex voor AT90S2333 en AT90S4333\n");
        printf ("8.hex voor ATmega8\n");
        printf ("voor de eeprom files wordt ook _eeprom.hex toegevoegd\n");
        printf ("GEEN SPATIES IN FILENAMEN!\n");
        fprintf (file, "../ezb/baan.blk\n");
        fprintf (file, "../microcont/blok\n");
        fprintf (file, "../microcont/io\n");
        fclose (file);
        file = fopen ("print.ini", "r");
    }
    fgets (temp, MAX_LENGTH, file);
    sscanf (temp, "%s", blkFile); // scanf om van CR en LF aftekomen
    fgets (temp, MAX_LENGTH, file);
    sscanf (temp, "%s", blokFile);
    fgets (temp, MAX_LENGTH, file);
    sscanf (temp, "%s", ioFile);


    printf ("Gebruik\n");
    printf ("blkFile:          %s\n", blkFile);
    printf ("blokFile:         %s\n", blokFile);
    printf ("ioFile:           %s\n", ioFile);

    file = fopen (blkFile, "r");


    do
    {
        printf ("a enkel adres data commandos\n");
        printf ("i enkel IO adres data commandos\n");
        printf ("b een blok print test\n");
        printf ("p program een blok/IO printje\n");
        printf ("e program eeprom van blok/IO printje\n");
        printf ("w wissel print test\n");
        printf ("s stop dit prorgramma\n");

        fgets (temp, MAX_LENGTH, stdin);

//      scanf ("%c", &command);
        switch (temp[0])
        {
        case 'a':
            // adres data test
            adresDataTest ();
            break;
        case 'b':
            // adres data test
            blokPrintTest ();
            break;
        case 'e':
            // program centrale controller
            programEeprom (file);
            break;
        case 'i':
            // adres data test
            adresIODataTest ();
            break;
        case 'p':
            // adres data test
            programBlokIOPrint ();
            break;
        case 's':
            // stop ik hoef niets te doen
            break;
        case 't':
            tijdelijkeTest();
            break;
        case 'w':
            wisselPrintTest();
            break;

        default:
            printf ("Onbekend commando\n");
        }
        fflush (stdin);
    }
    while (temp[0] != 's');

    return 0;
}
