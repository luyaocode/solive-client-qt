#ifndef ISOCKETSTRATEGY_H
#define ISOCKETSTRATEGY_H
#include <string>
namespace sio
{
    class client;
}

namespace SoLive::ProtocolSocketClient
{
    class ISocketStrategy
    {
    public:
        virtual ~ISocketStrategy() = default;
        virtual void connect(sio::client& client,const std::string& uri) = 0;
    };
} // namespace ProtocolSocketClient

#endif // ISOCKETSTRATEGY_H
