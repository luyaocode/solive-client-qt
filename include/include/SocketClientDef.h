// SocketClientDef.h
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

    constexpr auto STR_UNCONNECTED = "未连接";
    constexpr auto STR_CONNECTED = "已连接";
    constexpr auto STR_CONNECTING = "连接中...";
}

#endif // CONNECTIONSTATE_H
