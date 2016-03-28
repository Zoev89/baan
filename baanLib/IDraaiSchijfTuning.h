#ifndef IDRAAISCHIJFTUNING_H
#define IDRAAISCHIJFTUNING_H

class IDraaiSchijfTuning
{
  // PASOP de implementatie is niet reenterend!
  // nieuwItem en krijgItem kunnen wel van
  // twee processen gecalled worden
  // Construction
public:
  virtual void show(int basisHardwareAdres) = 0;	// zet een nieuwItem in de array return 1 dan is die vol
protected:
    virtual ~IDraaiSchijfTuning() {}

};


#endif // IDRAAISCHIJFTUNING_H
