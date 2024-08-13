#include "HttpsSocketStrategy.h"
#include <iostream>
#include <sio_client.h>
#include <Logger.h>

namespace SoLive::ProtocolSocketClient
{

    void HttpsSocketStrategy::connect(sio::client& client,const std::string& uri)
    {
        LOG(Info, "Connecting to " + uri + " using HTTPS.")
    }

} // namespace ProtocolSocketClient
