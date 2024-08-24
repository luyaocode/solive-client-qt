#ifndef HTTPSSOCKETSTRATEGY_H
#define HTTPSSOCKETSTRATEGY_H

#include "ISocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{

    class HttpsSocketStrategy : public ISocketStrategy
    {
    public:
        void connect(ISocket& client,const std::string& uri) override;
    };

} // namespace ProtocolSocketClient

#endif // HTTPSSOCKETSTRATEGY_H
