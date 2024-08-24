#ifndef HTTPSOCKETSTRATEGY_H
#define HTTPSOCKETSTRATEGY_H

#include "ISocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{
    class HttpSocketStrategy : public ISocketStrategy
    {
    public:
        void connect(ISocket& client,const std::string& uri) override;
    };

} // namespace ProtocolSocketClient

#endif // HTTPSOCKETSTRATEGY_H
