// ISocket.h
#pragma once
#include <string>
#include <variant>
#include <sio_client.h>

using EventVariant = std::variant<std::string, sio::event>;

namespace SoLive::ProtocolSocketClient
{
    class ISocket
    {
    public:
        virtual void connect(const std::string& url) = 0;
        virtual void emit(const std::string& eName, const std::string& msg) = 0;
        virtual void listen(const std::string& eName, const std::function<void(const EventVariant& event)>& callback) = 0;
        virtual void close() = 0;
        virtual std::string id() = 0;
    };
}