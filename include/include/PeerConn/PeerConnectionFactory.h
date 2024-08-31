#ifndef PEER_CONNECTION_FACTORY_H
#define PEER_CONNECTION_FACTORY_H

#include <rtc_base/thread.h>
#include <api/scoped_refptr.h>
#include <api/location.h>
#include <memory>
namespace webrtc
{
    class  PeerConnectionFactoryInterface;
}

namespace SoLive::PeerConnection
{
    class PeerConnectionFactory
    {
    public:
        // 获取单例实例
        static PeerConnectionFactory& getInstance();
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> getFactory();
        rtc::Thread* signalingThread() { return _signalingThread.get(); }

    private:
        PeerConnectionFactory();
        ~PeerConnectionFactory() = default;
        void initialize();
        std::unique_ptr<rtc::Thread> _networkThread{nullptr};
        std::unique_ptr<rtc::Thread> _workerThread{nullptr};
        std::unique_ptr<rtc::Thread> _signalingThread{nullptr};
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory{nullptr};
    };

}

#endif // PEER_CONNECTION_FACTORY_H
