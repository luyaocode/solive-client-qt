#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <string>
#include "ThreadSafeQueue.h"


namespace SoLive::Util
{
    class MessageManager
    {
    public:
        static MessageManager& instance();
        void push(const std::string& message);
        bool pop(std::string& message);
    private:
        MessageManager() = default;
        ~MessageManager() = default;
        MessageManager(const MessageManager&) = delete;
        MessageManager& operator=(const MessageManager&) = delete;
        ThreadSafeQueue<std::string> _msgQueue;
    };
}

#define MSG_PUSH(str) SoLive::Util::MessageManager::instance().push((str));

#endif // MESSAGEMANAGER_H
