#include "HttpSocketStrategy.h"
#include <iostream>
#include <sio_client.h>

namespace SoLive::ProtocolSocketClient
{
    void HttpSocketStrategy::connect(sio::client& client,const std::string& uri)
    {
        std::cout << "Connecting to " << uri<< " using HTTP." << std::endl;
        client.connect(uri);
    }

} // namespace ProtocolSocketClient
