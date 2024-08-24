#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "ISocketStrategy.h"
#include <memory>
#include <vector>
#include <QString>
#include <QJsonObject>
#include <functional>
#include <mutex>
#include "SocketClientDef.h"
#include "ISocket.h"

namespace SoLive::ProtocolSocketClient
{
    class ISocketClientObserver;

    class SocketClient
    {
    private:
        std::mutex _mtx;
        std::unique_ptr<ISocketStrategy>    _strategy;
        std::unique_ptr<ISocket>            _client;
        ConnectionState                     _connState;
        int                                 _onlinePersonNum;
        std::vector<std::shared_ptr<ISocketClientObserver>> _observers;
    private:
        SocketClient();
        ~SocketClient() = default;
        SocketClient(const SocketClient&) = delete;
        SocketClient& operator=(const SocketClient&) = delete;
        void notifyObservers(EventType eventType);
        void setListeners();
        void setState(ConnectionState newState);
    public:
        static SocketClient& getInstance();
        template<typename PTy=std::shared_ptr<QJsonObject>>
        void emit(const QString& eName , PTy jsonObj=nullptr);
        const std::string& socketId() const;
        void setStrategy(std::unique_ptr<ISocketStrategy> newStrategy);
        void connect(const std::string& uri);
        void listen(const std::string& eName, const std::function<void(const EventVariant& event)>& callback);
        ConnectionState getState() const;
        void setOnlinePersonNum(int num);
        int getOnlinePersonNum() const;
        void addObserver(const std::shared_ptr<ISocketClientObserver>& observer);
        void removeObserver(const std::shared_ptr<ISocketClientObserver>& observer);
    };
} // namespace ProtocolSocketClient
#include "SocketClientImpl.h"

#endif // SOCKETCLIENT_H
