#ifndef IHARDWARECOM_H
#define IHARDWARECOM_H

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


class IHardwareCom
{
  // PASOP de implementatie is niet reenterend!
  // nieuwItem en krijgItem kunnen wel van
  // twee processen gecalled worden
  // Construction
public:
  virtual int nieuwItem (const hardwareArray_t & data) = 0;	// zet een nieuwItem in de array return 1 dan is die vol
  virtual int krijgItem (hardwareArray_t & data) = 0;	// krijg het oudste item uit de array return 1 als leeg
  virtual int aantalItems () = 0;
protected:
    virtual ~IHardwareCom() {}

};

#endif // IHARDWARECOM_H
