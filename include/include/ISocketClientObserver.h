// IConnectionStateObserver.h
#ifndef ISOCKETCLIENTOBSERVER_H
#define ISOCKETCLIENTOBSERVER_H

namespace SoLive::ProtocolSocketClient
{
    enum class ConnectionState;

    class ISocketClientObserver
    {
    public:
        virtual ~ISocketClientObserver() = default;
        virtual void onConnectionStateChanged(ConnectionState newState)=0;
        virtual void onOnlinePersonNumberChanged(int num)=0;
    };
}

#endif // ISOCKETCLIENTOBSERVER_H
