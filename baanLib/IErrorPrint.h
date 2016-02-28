#ifndef IERRORPRINT_H
#define IERRORPRINT_H

class IErrorPrint
{
public:
    ~IErrorPrint() = default;

    virtual void errorPrint(const char * msg,...) = 0;
    virtual void errorPrintClose(void) = 0;
};

#endif // IERRORPRINT_H
