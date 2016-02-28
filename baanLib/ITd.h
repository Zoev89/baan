#ifndef ITD_H
#define ITD_H
#include <string>

class ITd
{
public:
    ~ITd() = default;

    virtual int tdCreate (const std::string& string) = 0;

    virtual void tdStart (int id) = 0;
    virtual void tdStop (int id) = 0;

    virtual int tdValueString (const std::string& string) = 0;
    // print een getal in de logging
    // precondition is het aanmelden met tdValueString
    virtual void tdValue (int stringId, int value) = 0;

    virtual void tdDump () = 0;
};

#endif // ITD_H
