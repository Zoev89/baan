#ifndef _HARDWARECOM_H_
#define _HARDWARECOM_H_
#include "IHardwareCom.h"


class HardwareCom: public IHardwareCom
{
  // PASOP de implementatie is niet reenterend!
  // nieuwItem en krijgItem kunnen wel van
  // twee processen gecalled worden
  // Construction
public:
  explicit HardwareCom (bool editMode);		// constructor de ~ChardwareCom descructor is niet nodig
  // IHardwareCom
  virtual int nieuwItem (const hardwareArray_t & data) override;	// zet een nieuwItem in de array return 1 dan is die vol
  virtual int krijgItem (hardwareArray_t & data) override;	// krijg het oudste item uit de array return 1 als leeg
  virtual int aantalItems () override;
protected:

private:
  int kop;			// kop wijst altijd naar een lege plek
  int staart;
  int maxLengte;
  hardwareArray_t array[MAX_AANTAL_AANVRAGEN];
  bool m_editMode;
};

#endif // _HARDWARECOM_H_

