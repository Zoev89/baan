#ifndef _HARDWARECOM_H_
#define _HARDWARECOM_H_

#define MAX_AANTAL_AANVRAGEN  40

typedef enum
{
  HW_BLOK,
  HW_IO
} hardwareBIO_t;

typedef struct
{
  hardwareBIO_t blokIO;
  int adres;			// -1 als het niet in gebruik is
  int data;
  // het nummer in de blok of wisselSpoelArray ook altijd invullen (ivm kortsluit test)!
  int nummer;
  int returnGewenst;
} hardwareArray_t;


class ChardwareCom
{
  // PASOP de implementatie is niet reenterend!
  // nieuwItem en krijgItem kunnen wel van
  // twee processen gecalled worden
  // Construction
public:
  ChardwareCom ();		// constructor de ~ChardwareCom descructor is niet nodig
  int nieuwItem (hardwareArray_t * data);	// zet een nieuwItem in de array return 1 dan is die vol
  int krijgItem (hardwareArray_t * data);	// krijg het oudste item uit de array return 1 als leeg
  int aantalItems ();
protected:

private:
  int kop;			// kop wijst altijd naar een lege plek
  int staart;
  int maxLengte;
  hardwareArray_t array[MAX_AANTAL_AANVRAGEN];
};

#endif // _HARDWARECOM_H_

