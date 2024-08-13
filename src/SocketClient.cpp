#include "SocketClient.h"
#include <iostream>
#include "ISocketClientObserver.h"
#include "Logger.h"
#include <string>

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
        return _client.get_sessionid();
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

    void SocketClient::listen(const std::string& eName, const std::function<void(sio::event&)>& callback)
    {
        _client.socket()->on(eName, callback);
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
        // �������ӳɹ��ļ�����
        _client.set_open_listener([this]()
            {
                setState(ConnectionState::Connected);
                LOG(Info, "Connection opened!")
            });

        // ��������ʧ�ܵļ�����
        _client.set_fail_listener([this]()
            {
                setState(ConnectionState::Disconnected);
                LOG(Warning, "Connection failed!")
            });

        // ���������еļ�����
        _client.set_reconnecting_listener([this]()
            {
                setState(ConnectionState::Reconnecting);
                LOG(Info, "Reconnecting...")
            });

        // ���������ļ�����
        _client.set_reconnect_listener([this](unsigned attempts, unsigned delay)
            {
                setState(ConnectionState::Reconnecting);
                auto strInfo = "Reconnected after " + std::to_string(attempts) + " attempts with delay " + std::to_string(delay) + "ms";
                LOG(Info, strInfo)
            });

        // �������ӹرյļ�����
        _client.set_close_listener([this](sio::client::close_reason const& reason)
            {
                setState(ConnectionState::Disconnected);
                LOG(Info, "Connection closed! Reason: " + reason)
            });

        // ���� Socket �򿪵ļ�����
        _client.set_socket_open_listener([this](const std::string& nsp)
            {
                LOG(Info, "Socket opened in namespace: " + nsp)
            });

        // ���� Socket �رյļ�����
        _client.set_socket_close_listener([this](const std::string& nsp)
            {
                LOG(Info, "Socket closed in namespace: " + nsp)
            });

        // ������ǰ��������
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
                    LOG(Warning, "Unexpected message type.")
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
