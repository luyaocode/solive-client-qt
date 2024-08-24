#include "stdafx.h"
#include "HttpSocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{
    void HttpSocketStrategy::connect(ISocket& client,const std::string& uri)
    {
        LOG(Info,"Connecting to " +uri+" using HTTP.")
        client.connect(uri);
    }

} // namespace ProtocolSocketClient
