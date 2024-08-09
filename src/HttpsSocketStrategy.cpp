#include "HttpsSocketStrategy.h"
#include <iostream>
#include <sio_client.h>

namespace SoLive::ProtocolSocketClient
{

    void HttpsSocketStrategy::connect(sio::client& client,const std::string& uri)
    {
        std::cout << "Connecting to " <<uri << " using HTTPS." << std::endl;
    }

} // namespace ProtocolSocketClient
