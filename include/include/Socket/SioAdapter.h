// SioAdapter.h
#pragma once
#include "ISocket.h"
#include <sio_client.h>

namespace SoLive::ProtocolSocketClient
{
    class SioAdapter : public ISocket
    {
    public:
        void connect(const std::string& url)override;
        void emit(const std::string& eName, const std::string& msg) override;
        void listen(const std::string& eName, const std::function<void(const EventVariant& event)>& callback) override;
        void close() override;
        std::string id() override;
        sio::client& client() { return _client; };

    private:
        sio::client _client;
    };
}
