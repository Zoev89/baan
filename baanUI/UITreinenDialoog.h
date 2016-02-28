#ifndef UITREINENDIALOOG_H
#define UITREINENDIALOOG_H
#include "ITreinenDialoog.h"

class UITreinenDialoog : public ITreinenDialoog
{
public:
    UITreinenDialoog();

    virtual void CreateTreinen (int x, int y, int width, int height);
    virtual void AddTrein (const std::string& soort, const std::string& name, int regelaarNummer, const std::string& imageName);
    virtual void VerwijderTrein (int regelaarNummer);
    virtual void DestroyTreinen();
};

#endif // UITREINENDIALOOG_H
