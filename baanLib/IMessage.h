#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <string>

class IMessage
{
public:
    virtual ~IMessage() {}
    virtual void message(const std::string msg) = 0;
};

#endif // IMESSAGE_H
