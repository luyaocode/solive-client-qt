#ifndef ISOCKETSTRATEGY_H
#define ISOCKETSTRATEGY_H
#include <string>
#include "ISocket.h"

namespace SoLive::ProtocolSocketClient
{
    class ISocketStrategy
    {
    public:
        virtual ~ISocketStrategy() = default;
        virtual void connect(ISocket& client,const std::string& uri) = 0;
    };
} // namespace ProtocolSocketClient

#endif // ISOCKETSTRATEGY_H
