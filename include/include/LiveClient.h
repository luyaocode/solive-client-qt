#ifndef LIVECLIENT_H
#define LIVECLIENT_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "libmediasoupclient/mediasoupclient.hpp"

namespace SoLive::LiveClient
{
    class LiveClient;

    class RecvTrpListener :public mediasoupclient::RecvTransport::Listener
    {
    public:
        virtual ~RecvTrpListener() = default;
        virtual std::future<void> OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters) override;
        virtual void OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState) override;
        void closeRecvTransport(LiveClient& liveClient);
    };

    class ConsumerListener :public mediasoupclient::Consumer::Listener
    {
    public:
        virtual ~ConsumerListener() = default;
        virtual void OnTransportClose(mediasoupclient::Consumer* consumer) override;
    };

    class LiveClient : public QObject
    {
        Q_OBJECT

    public:
        static LiveClient& getInstance();
        ~LiveClient();
        void init();
        void setupDevice();
        friend void RecvTrpListener::closeRecvTransport(LiveClient& liveClient);
        void closeRecvTransport();
        void consume();

        void addRemoteTrack(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

    Q_SIGNALS:
        void onRemoteVideoTrackReceived(webrtc::VideoTrackInterface* videoTrack);
        void onRemoteAudioTrackReceived(webrtc::AudioTrackInterface* audioTrack);
        void roomConnected(const QString& roomId);
        void sigClearWidget();

    private:
        explicit LiveClient(QObject* parent = nullptr);
        LiveClient(const LiveClient&) = delete;
        LiveClient& operator=(const LiveClient&) = delete;
        void setListener();
        void setupConnection();
    public Q_SLOTS:
        void showWarningMessageBox(const QString& roomId);
        void handleEnterRoom(const QString& newRoom, const QString& oldRoom);
        void handleLeaveRoom(const QString& roomId);
    private:
        std::mutex _mtx;
        std::condition_variable _cv;
        std::unique_ptr<mediasoupclient::Device> _devicePtr;
        std::unique_ptr<mediasoupclient::RecvTransport> _recvTransportPtr;
        std::vector<std::unique_ptr<mediasoupclient::Consumer>> _consumerVec;
        std::unique_ptr<RecvTrpListener> _recvTrpListenerPtr;
        std::unique_ptr<ConsumerListener> _consumerListenerPtr;
        bool _bDeviceLoaded;
        bool _bRecvTrpCreated;
        QString _oldRoom;
    };
}
#endif // LIVECLIENT_H
