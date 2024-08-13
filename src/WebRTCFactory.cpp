#include "WebRTCFactory.h"

namespace SoLive::WebRTC
{
    rtc::scoped_refptr<webrtc::MediaStreamInterface> WebRTCFactory::createMediaStream(const std::string& stream_id)
    {
        return nullptr;
    }
}
