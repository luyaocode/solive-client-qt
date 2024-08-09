#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "ISocketStrategy.h"
#include <memory>
#include <vector>
#include <sio_client.h>
#include "SocketClientDef.h"

namespace SoLive::ProtocolSocketClient
{
    class ISocketClientObserver;

    class SocketClient
    {
    private:
        std::unique_ptr<ISocketStrategy> _strategy;
        sio::client _client;
        ConnectionState _connState;
        int _onlinePersonNum;
        std::vector<std::shared_ptr<ISocketClientObserver>> _observers;
    private:
        SocketClient();
        ~SocketClient() = default;
        SocketClient(const SocketClient&) = delete;
        SocketClient& operator=(const SocketClient&) = delete;
        void notifyObservers(EventType eventType);
    public:
        static SocketClient& getInstance();
        void setStrategy(std::unique_ptr<ISocketStrategy> newStrategy);
        void connect(const std::string& uri);
        void setListeners();
        void setState(ConnectionState newState);
        ConnectionState getState() const;
        void setOnlinePersonNum(int num);
        int getOnlinePersonNum() const;
        void addObserver(const std::shared_ptr<ISocketClientObserver>& observer);
        void removeObserver(const std::shared_ptr<ISocketClientObserver>& observer);
    };

} // namespace ProtocolSocketClient

#endif // SOCKETCLIENT_H
