#ifndef UIMESSAGE_H
#define UIMESSAGE_H
#include "IMessage.h"

class UIMessage : public IMessage
{
public:
    UIMessage();

    virtual void message(const std::string msg);

};

#endif // UIMESSAGE_H
