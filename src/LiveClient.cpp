#include "LiveClient.h"
#include <QDebug>

namespace SoLive::LiveClient
{

    LiveClient& LiveClient::getInstance()
    {
        static LiveClient instance = LiveClient();
        return instance;
    }

    LiveClient::LiveClient(QObject* parent)
        : QObject(parent)
    {
        mediasoupclient::Initialize();
    }

    LiveClient::~LiveClient()
    {
        mediasoupclient::Cleanup();
    }

    void LiveClient::connectToServer(const std::string& signalingServerUrl)
    {
        // 连接信令服务器的逻辑实现
        setupTransport();
    }

    void LiveClient::joinRoom(const std::string& roomId)
    {
        // 使用_device_加入房间，并创建_RecvTransport_
    }

    void LiveClient::addRemoteTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
    {
        if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
        {
            auto videoTrack = rtc::scoped_refptr<webrtc::VideoTrackInterface>(
                dynamic_cast<webrtc::VideoTrackInterface*>(track.get())
                );
            if (videoTrack)
            {
                onRemoteVideoTrackReceived(videoTrack);
            }
            else
            {
                qDebug() << "Failed to cast track to VideoTrackInterface";
            }
        }
    }

    void LiveClient::setupTransport()
    {
        // 配置并创建 _recvTransport_ 的逻辑
    }
}
