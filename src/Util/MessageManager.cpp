#include "stdafx.h"
#include "MessageManager.h"

using namespace SoLive::Util;

MessageManager& MessageManager::instance()
{
    static MessageManager instance;
    return instance;
}

void MessageManager::push(const std::string& message)
{
    _msgQueue.push(message);
}

bool MessageManager::pop(std::string& message)
{
    if (_msgQueue.isEmpty())
    {
        return false;
    }
    _msgQueue.wait_and_pop(message);
    return true;
}
