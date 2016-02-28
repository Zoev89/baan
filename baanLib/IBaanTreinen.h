#ifndef IBAANTREINEN_H
#define IBAANTREINEN_H
#include <string>

class IBaanTreinen
{
public:
    virtual void baanCreateTreinen (int x, int y, int width, int height) = 0;
    virtual void baanAddTrein (char *soort,char *name, int regelaarNummer, const std::string& imageName) = 0;
    virtual void baanDestroyTreinen() = 0;
    virtual void baanVerwijderTrein (int regelaarNummer) = 0;
    virtual void baanTreinenActivateRegelaar (int regelaarNummer)= 0;
};

#endif // IBAANTREINEN_H
