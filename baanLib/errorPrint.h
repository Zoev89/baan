#ifndef ERRORPRINT_H_
#define ERRORPRINT_H_

#include <stdarg.h>
#include "IErrorPrint.h"

class ErrorPrint : public IErrorPrint
{
public:
    ErrorPrint(int &tickTimer);
    virtual void errorPrint(const char * msg,...);
    virtual void errorPrintClose(void);
private:
    int & mTickTimer;
};

#endif /*ERRORPRINT_H_*/
