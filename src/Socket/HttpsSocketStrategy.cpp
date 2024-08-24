#include "stdafx.h"
#include "HttpsSocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{

    void HttpsSocketStrategy::connect(ISocket& client,const std::string& uri)
    {
        LOG(Info, "Connecting to " + uri + " using HTTPS.")
        client.connect(uri);
    }

} // namespace ProtocolSocketClient
