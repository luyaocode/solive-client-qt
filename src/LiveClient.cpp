#include "LiveClient.h"
#include <QDebug>
#include <QJsonObject>
#include <string>
#include <QMessageBox>
#include <QApplication>
#include <QSignalSpy>
#include "nlohmann/json.hpp"
#include "SocketClient.h"
#include "LiveViewerPageDef.h"
#include "Logger.h"

namespace SoLive::LiveClient
{
    std::future<void> RecvTrpListener::OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters)
    {
        std::promise<void> promise;
        auto& socketClient = SoLive::ProtocolSocketClient::SocketClient::getInstance();
        auto jsonPtr = std::make_unique<QJsonObject>();
        QString qstrId = transport->GetId().c_str();
        (*jsonPtr)["from"] = "qt_client";
        (*jsonPtr)["transportId"] = qstrId;
        (*jsonPtr)["dtlsParameters"] = dtlsParameters.dump().c_str();
        socketClient.emit(SoLive::Page::EVENT_CONNECT_CONS_TRP,std::move(jsonPtr));

        promise.set_value();
        return promise.get_future();
    }

    void RecvTrpListener::OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState)
    {
        if (connectionState == "new")
        {
            LOG(Info, "Transport new.")
        }
        else if (connectionState == "connecting")
        {
            LOG(Info,"Transport connecting...")
        }
        else if (connectionState == "connected")
        {
            auto& socketClient = SoLive::ProtocolSocketClient::SocketClient::getInstance();
            auto jsonObj = std::make_unique<QJsonObject>();
            socketClient.emit(SoLive::Page::EVENT_RESUME, std::move(jsonObj));
            LOG(Info, "Transport connected.")
        }
        else if (connectionState == "disconnected")
        {
            LOG(Info, "Transport disconnected.")
        }
        else if (connectionState == "failed")
        {
            LOG(Info, "Transport failed to connect.")
        }
        else if(connectionState == "closed")
        {
            closeRecvTransport(LiveClient::getInstance());
            LOG(Info, "Transport closed.")
        }
    }

    void RecvTrpListener::closeRecvTransport(LiveClient& liveClient)
    {
        liveClient.closeRecvTransport();
    }

    void ConsumerListener::OnTransportClose(mediasoupclient::Consumer* consumer)
    {
        LOG(Info,"Transport closed.")
    }


    LiveClient& LiveClient::getInstance()
    {
        static LiveClient instance = LiveClient();
        return instance;
    }

    LiveClient::LiveClient(QObject* parent)
        : QObject(parent)
    {
        mediasoupclient::Initialize();
        setupDevice();
        setListener();
    }

    LiveClient::~LiveClient()
    {
        mediasoupclient::Cleanup();
    }

    void LiveClient::addRemoteTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
    {
        if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
        {
            auto videoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
                dynamic_cast<webrtc::VideoTrackInterface*>(track.get())
                );
            onRemoteVideoTrackReceived(videoTrack.get());
        }
        else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind)
        {
            auto audioTrack = rtc::scoped_refptr<webrtc::AudioTrackInterface>(
                dynamic_cast<webrtc::AudioTrackInterface*>(track.get())
                );
            onRemoteAudioTrackReceived(audioTrack.get());
        }
    }

    void LiveClient::setupDevice()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _devicePtr = std::make_unique<mediasoupclient::Device>(mediasoupclient::Device());
        _bDeviceLoaded = true;
        _cv.notify_all();
    }

    void LiveClient::closeRecvTransport()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        if (_bRecvTrpCreated)
        {
            _recvTransportPtr->Close();
            _bRecvTrpCreated = false;
        }
    }

    void LiveClient::consume()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _cv.wait(lock, [&]() { return _bDeviceLoaded && _bRecvTrpCreated; });
        auto& socketClient = SoLive::ProtocolSocketClient::SocketClient::getInstance();
        auto rtpCapas = _devicePtr->GetRtpCapabilities();
        auto strRtpCpas = rtpCapas.dump();

        auto jsonObj = std::make_unique<QJsonObject>();
        (*jsonObj)["from"] = "qt_client";
        (*jsonObj)["data"] = strRtpCpas.c_str();
        socketClient.emit(SoLive::Page::EVENT_CONSUME, std::move(jsonObj));
        _cv.notify_all();
    }

    void LiveClient::setListener()
    {
        auto& socketClient = SoLive::ProtocolSocketClient::SocketClient::getInstance();
        socketClient.listen(SoLive::Page::EVENT_ROOM_NOT_EXIST,[this](sio::event& ev){
            auto message = ev.get_message();
            if (message->get_flag() == sio::message::flag_string)
            {
                auto roomId = message->get_string();
                QString st = QString(roomId.c_str());
                // 使用 Qt 的信号和槽机制确保在 GUI 线程中显示消息框
                QMetaObject::invokeMethod(this, "showWarningMessageBox", Qt::BlockingQueuedConnection,
                    Q_ARG(QString, st));
            }
            else
            {
                LOG(Warning, "Unexpected message type.")
            }
        });

        // 进入房间
        socketClient.listen(SoLive::Page::EVENT_ROOM_ENTERED, [&](sio::event& ev)
            {
                auto jsonObj = std::make_unique<QJsonObject>();
                (*jsonObj)["from"] = "qt_client";
                socketClient.emit(SoLive::Page::EVENT_GET_RTP_CAPA,std::move(jsonObj));
            });

        // 服务器路由RTP能力
        socketClient.listen(SoLive::Page::EVENT_RTP_CAPA_GOT, [&](sio::event& ev)
            {
                auto message = ev.get_message();
                auto flag = message->get_flag();
                if (flag == sio::message::flag_string)
                {
                    auto strJson = message->get_string();
                    nlohmann::json jsonObject = nlohmann::json::parse(strJson);
                    _devicePtr->Load(jsonObject);

                    auto jsonObj = std::make_unique<QJsonObject>();
                    (*jsonObj)["from"] = "qt_client";
                    socketClient.emit(SoLive::Page::EVENT_CREATE_CONS_TRP, std::move(jsonObj));
                }
                else
                {
                    LOG(Warning, "Unexpected message type.")
                }
            });

        // 消费者通道创建成功
        socketClient.listen(SoLive::Page::EVENT_CONS_TRP_CREATED, [&](sio::event& ev)
            {
                auto message = ev.get_message();
                auto flag = message->get_flag();
                if (flag == sio::message::flag_string)
                {
                    auto strJson = message->get_string();
                    nlohmann::json jsonObject = nlohmann::json::parse(strJson);
                    std::string strTrpId = jsonObject["id"].get<std::string>();
                    nlohmann::json iceParameters = jsonObject["iceParameters"].get<nlohmann::json>();
                    std::string strIceParameters = iceParameters.dump();
                    nlohmann::json iceCandidates = jsonObject["iceCandidates"].get<nlohmann::json>();
                    std::string strIceCandidates = iceCandidates.dump();
                    nlohmann::json dtlsParameters = jsonObject["dtlsParameters"].get<nlohmann::json>();
                    std::string strDtlsParameters = dtlsParameters.dump();
                    _recvTrpListenerPtr = std::make_unique<RecvTrpListener>();
                    mediasoupclient::RecvTransport* recvTrp = _devicePtr->CreateRecvTransport(
                        _recvTrpListenerPtr.get(),
                        strTrpId, 
                        iceParameters, 
                        iceCandidates, 
                        dtlsParameters);
                    _bRecvTrpCreated = true;

                    _recvTransportPtr = std::unique_ptr<mediasoupclient::RecvTransport>(recvTrp);
                    consume();
                }
                else
                {
                    LOG(Warning, "Unexpected message type.")
                }
            });

        // 消费者创建成功
        socketClient.listen(SoLive::Page::EVENT_SUBSCRIBED, [&](sio::event& ev)
            {
                auto message = ev.get_message();
                auto flag = message->get_flag();
                if (flag == sio::message::flag_string)
                {
                    auto strJson = message->get_string();
                    nlohmann::json jsonObject = nlohmann::json::parse(strJson);
                    for (nlohmann::json::iterator it = jsonObject.begin(); it != jsonObject.end(); ++it)
                    {
                        auto peer = it.key();
                        auto params = it.value();
                        for (nlohmann::json::iterator it1 = params.begin(); it1 != params.end(); ++it1)
                        {
                            auto producer = it1.key();
                            auto params1 = it1.value();
                            auto consumerId = params1["id"];
                            auto kind = params1["kind"];
                            auto rtpParameters = params1["rtpParameters"];
                            
                            _consumerListenerPtr = std::unique_ptr<ConsumerListener>();
                            auto consumer = _recvTransportPtr->Consume(
                                _consumerListenerPtr.get(),
                                consumerId,
                                producer,
                                kind,
                                &rtpParameters);
                            auto consumerPtr = std::unique_ptr<mediasoupclient::Consumer>(consumer);
                            _consumerVec.push_back(std::move(consumerPtr));
                        }
                    }
                }
                else
                {
                    LOG(Warning, "Unexpected message type.")
                }
            });

        // 消费者通道连接成功
        socketClient.listen(SoLive::Page::EVENT_CONS_TRP_CONNECTED, [&](sio::event& ev)
            {
                std::vector<rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> tracks;
                for (auto it = _consumerVec.begin(); it != _consumerVec.end(); ++it)
                {
                    auto id = (*it)->GetId();
                    auto bKind = (*it)->GetKind();
                    auto bClosed = (*it)->IsClosed();
                    auto bPaused = (*it)->IsPaused();
                    auto track = (*it)->GetTrack();
                    tracks.emplace_back(track);
                }

                for (auto& track : tracks)
                {
                    addRemoteTrack(track);
                }
            });


        // 消费者resume成功
        socketClient.listen(SoLive::Page::EVENT_RESUMED, [&](sio::event& ev)
            {
                auto message = ev.get_message();
                auto flag = message->get_flag();
                if (flag == sio::message::flag_string)
                {
                    auto roomId = message->get_string();
                    QString qstrRoomId = QString(roomId.c_str());
                    roomConnected(qstrRoomId);
                }
                else
                {
                    LOG(Warning, "Unexpected message type.")
                }
            });
    }

    void LiveClient::showWarningMessageBox(const QString& roomId)
    {
        QMessageBox::warning(nullptr, "警告", "直播间 " + roomId + " 不存在");
    }
}
