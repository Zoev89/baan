#ifndef ITREINENDIALOG_H
#define ITREINENDIALOG_H
#include <string>

class ITreinenDialoog
{
public:
    ~ITreinenDialoog() = default;

    virtual void CreateTreinen (int x, int y, int width, int height) = 0;
    virtual void AddTrein (const std::string& soort, const std::string& name, int regelaarNummer, const std::string& imageName) = 0;
    virtual void VerwijderTrein (int regelaarNummer) = 0;
    virtual void DestroyTreinen() = 0;
};

#endif // ITREINENDIALOG_H
