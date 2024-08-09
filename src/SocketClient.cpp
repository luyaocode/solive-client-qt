#include "SocketClient.h"
#include <iostream>
#include "ISocketClientObserver.h"

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

    void SocketClient::setStrategy(std::unique_ptr<ISocketStrategy> newStrategy)
    {
        _strategy = std::move(newStrategy);
    }

    void SocketClient::connect(const std::string& uri)
    {
        if (_strategy)
        {
            _strategy->connect(_client,uri);
            setListeners();
        }
        else
        {
            std::cerr << "Strategy not set!" << std::endl;
        }
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
        // 设置连接成功的监听器
        _client.set_open_listener([this]()
            {
                setState(ConnectionState::Connected);
                std::cout << "Connection opened!" << std::endl;
            });

        // 设置连接失败的监听器
        _client.set_fail_listener([this]()
            {
                setState(ConnectionState::Disconnected);
                std::cout << "Connection failed!" << std::endl;
            });

        // 设置重连中的监听器
        _client.set_reconnecting_listener([this]()
            {
                setState(ConnectionState::Reconnecting);
                std::cout << "Reconnecting..." << std::endl;
            });

        // 设置重连的监听器
        _client.set_reconnect_listener([this](unsigned attempts, unsigned delay)
            {
                setState(ConnectionState::Reconnecting);
                std::cout << "Reconnected after " << attempts << " attempts with delay " << delay << "ms" << std::endl;
            });

        // 设置连接关闭的监听器
        _client.set_close_listener([this](sio::client::close_reason const& reason)
            {
                setState(ConnectionState::Disconnected);
                std::cout << "Connection closed! Reason: " << reason << std::endl;
            });

        // 设置 Socket 打开的监听器
        _client.set_socket_open_listener([this](const std::string& nsp)
            {
                std::cout << "Socket opened in namespace: " << nsp << std::endl;
            });

        // 设置 Socket 关闭的监听器
        _client.set_socket_close_listener([this](const std::string& nsp)
            {
                std::cout << "Socket closed in namespace: " << nsp << std::endl;
            });

        // 监听当前在线人数
        _client.socket()->on("currentHeadCount", [this](sio::event& ev)
            {
                auto message = ev.get_message();
                if (message->get_flag() == sio::message::flag_integer)
                {
                    int headCount = message->get_int();
                    setOnlinePersonNum(headCount);
                }
                else
                {
                    std::cout << "Unexpected message type." << std::endl;
                }
            });
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
        auto it = std::find(_observers.begin(), _observers.end(), observer);
        if (it == _observers.end())
        {
            _observers.push_back(observer);
        }
    }

    void SocketClient::removeObserver(const std::shared_ptr<ISocketClientObserver>& observer)
    {
        _observers.erase(
            std::remove(_observers.begin(), _observers.end(), observer),
            _observers.end()
        );
    };

} // namespace ProtocolSocketClient
