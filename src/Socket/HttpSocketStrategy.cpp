#include "stdafx.h"
#include "HttpSocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{
    void HttpSocketStrategy::connect(sio::client& client,const std::string& uri)
    {
        LOG(Info,"Connecting to " +uri+" using HTTP.")
        client.connect(uri);
    }

} // namespace ProtocolSocketClient
