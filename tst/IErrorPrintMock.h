#ifndef IERRORPRINTMOCK_H
#define IERRORPRINTMOCK_H
#include "IErrorPrint.h"

// IK kan geen variable arguments mocken dus implementeer ik de class maar
// Kan dus niet testen op de calls

class IErrorPrintMock : public IErrorPrint
{
public:
    virtual void errorPrint(const char * ,...){};
    virtual void errorPrintClose(void) {};

};

#endif // IERRORPRINTMOCK_H
