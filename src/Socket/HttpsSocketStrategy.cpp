#include "stdafx.h"
#include "HttpsSocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{

    void HttpsSocketStrategy::connect(sio::client& client,const std::string& uri)
    {
        LOG(Info, "Connecting to " + uri + " using HTTPS.")
    }

} // namespace ProtocolSocketClient
