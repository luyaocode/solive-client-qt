#include "stdafx.h"
#include "SocketClient.h"
#include "ISocketClientObserver.h"
#include "SioAdapter.h"
#include "BeastAdapter.h"
#include "HttpSocketStrategy.h"
#include "HttpsSocketStrategy.h"

namespace SoLive::ProtocolSocketClient
{
    SocketClient::SocketClient() :_connState(ConnectionState::Disconnected),
        _onlinePersonNum(0)
    {
    }

    SocketClient& SocketClient::getInstance()
    {
        static SocketClient instance;
        return instance;
    }

    const std::string& SocketClient::socketId() const
    {
        return _client->id();
    }

    void SocketClient::setStrategy(std::unique_ptr<ISocketStrategy> newStrategy)
    {
        _strategy = std::move(newStrategy);
        if (auto strategy = dynamic_cast<HttpSocketStrategy*>(_strategy.get()))
        {
            _client = std::make_unique<SioAdapter>();
        }
        else
        {
            //_client = std::make_unique<BeastAdapter>(new BeastAdapter);
        }
        setListeners();
    }

    void SocketClient::connect(const std::string& uri)
    {
        if (_strategy)
        {
            _strategy->connect(*_client, uri);
        }
        else
        {
            std::cerr << "Strategy not set!" << std::endl;
        }
    }

    void SocketClient::listen(const std::string& eName, const std::function<void(const EventVariant& event)>& callback)
    {
        _client->listen(eName, callback);
    }

    void SocketClient::notifyObservers(EventType eventType)
    {
        for (auto observer : _observers)
        {
            switch (eventType)
            {
            case EventType::ConnectionStateChange:
                observer->onConnectionStateChanged(_connState);
                break;
            case EventType::OnlinePersonNumChange:
                observer->onOnlinePersonNumberChanged(_onlinePersonNum);
                break;
            default:
                break;
            }
        }
    }

    void SocketClient::setListeners()
    {
        SioAdapter* cli = dynamic_cast<SioAdapter*>(_client.get());
        if (cli)
        {
            // 设置连接成功的监听器
            cli->client().set_open_listener([this]()
                {
                    setState(ConnectionState::Connected);
            LOG(Info, "Connection opened!")
                });

            // 设置连接失败的监听器
            cli->client().set_fail_listener([this]()
                {
                    setState(ConnectionState::Disconnected);
            LOG(Warning, "Connection failed!")
                });

            // 设置重连中的监听器
            cli->client().set_reconnecting_listener([this]()
                {
                    setState(ConnectionState::Reconnecting);
            LOG(Info, "Reconnecting...")
                });

            // 设置重连的监听器
            cli->client().set_reconnect_listener([&](unsigned attempts, unsigned delay)
                {
                    //cli->client().socket()->emit("hello");
                    setState(ConnectionState::Reconnecting);
                    auto strInfo = "Reconnected after " + std::to_string(attempts) + " attempts with delay " + std::to_string(delay) + "ms";
                    LOG(Info, strInfo)
                });

            // 设置连接关闭的监听器
            cli->client().set_close_listener([this](sio::client::close_reason const& reason)
                {
                    setState(ConnectionState::Disconnected);
            LOG(Info, "Connection closed! Reason: " + reason)
                });

            // 设置 Socket 打开的监听器
            cli->client().set_socket_open_listener([this](const std::string& nsp)
                {
                    LOG(Info, "Socket opened in namespace: " + nsp)
                });

            // 设置 Socket 关闭的监听器
            cli->client().set_socket_close_listener([this](const std::string& nsp)
                {
                    LOG(Info, "Socket closed in namespace: " + nsp)
                });

            // 监听当前在线人数
            cli->client().socket()->on("currentHeadCount", [this](sio::event& ev)
                {
                    auto message = ev.get_message();
                    if (message->get_flag() == sio::message::flag_integer)
                    {
                        int headCount = message->get_int();
                        setOnlinePersonNum(headCount);
                    }
                    else
                    {
                        LOG(Warning, "Unexpected message type.")
                    }
                });
        }
        else
        {

        }
    }

    void SocketClient::setState(ConnectionState newState)
    {
        if (_connState != newState)
        {
            _connState = newState;
            notifyObservers(EventType::ConnectionStateChange);
            if (newState == ConnectionState::Disconnected)
            {
                setOnlinePersonNum(0);
            }
        }
    };

    ConnectionState SocketClient::getState() const
    {
        return _connState;
    }

    void SocketClient::setOnlinePersonNum(int num)
    {
        if (_onlinePersonNum !=num )
        {
            _onlinePersonNum = num;
            notifyObservers(EventType::OnlinePersonNumChange);
        }
    }

    int SocketClient::getOnlinePersonNum() const
    {
        return _onlinePersonNum;
    }

    void SocketClient::addObserver(const std::shared_ptr<ISocketClientObserver>& observer)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        auto it = std::find(_observers.begin(), _observers.end(), observer);
        if (it == _observers.end())
        {
            _observers.push_back(observer);
        }
    }

    void SocketClient::removeObserver(const std::shared_ptr<ISocketClientObserver>& observer)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _observers.erase(
            std::remove(_observers.begin(), _observers.end(), observer),
            _observers.end()
        );
    };

} // namespace ProtocolSocketClient
