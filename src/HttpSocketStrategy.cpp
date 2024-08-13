#include "HttpSocketStrategy.h"
#include <iostream>
#include <sio_client.h>
#include "Logger.h"

namespace SoLive::ProtocolSocketClient
{
    void HttpSocketStrategy::connect(sio::client& client,const std::string& uri)
    {
        LOG(Info,"Connecting to " +uri+" using HTTP.")
        client.connect(uri);
    }

} // namespace ProtocolSocketClient
