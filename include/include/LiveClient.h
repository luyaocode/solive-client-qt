#ifndef LIVECLIENT_H
#define LIVECLIENT_H

#include <QWidget>
#include <QObject>
#include "libmediasoupclient/mediasoupclient.hpp"

namespace SoLive::LiveClient
{
    class LiveClient : public QObject
    {
        Q_OBJECT

    public:
        static LiveClient& getInstance();
        ~LiveClient();

        void connectToServer(const std::string& signalingServerUrl);
        void joinRoom(const std::string& roomId);
        void addRemoteTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

    Q_SIGNAL
        void onRemoteVideoTrackReceived(rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack);

    private:
        explicit LiveClient(QObject* parent = nullptr);
        LiveClient(const LiveClient&) = delete;
        LiveClient& operator=(const LiveClient&) = delete;
        void setupTransport();
    private:
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection;
        mediasoupclient::Device _device;
        std::unique_ptr<mediasoupclient::RecvTransport> _recvTransport;
    };
}
#endif // LIVECLIENT_H
