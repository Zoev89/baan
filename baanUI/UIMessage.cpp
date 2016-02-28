#include "UIMessage.h"
#include <iostream>

UIMessage::UIMessage()
{
}

void UIMessage::message(const std::string msg)
{
    std::cout << msg << std::endl;
}
