// ConnectionState.h
#ifndef SOCKETCLIENTDEF_H
#define SOCKETCLIENTDEF_H

namespace SoLive::ProtocolSocketClient
{
    enum class ConnectionState
    {
        Connected,
        Disconnected,
        Reconnecting
    };

    enum class EventType
    {
        ConnectionStateChange=1,
        OnlinePersonNumChange,
    };
}

#endif // SOCKETCLIENTDEF_H
