#ifndef WEBRTCFACTORY_H
#define WEBRTCFACTORY_H

#include "api/media_stream_interface.h"
#include "rtc_base/ref_counted_object.h"

namespace SoLive::WebRTC
{
    class WebRTCFactory
    {
    public:
        static rtc::scoped_refptr<webrtc::MediaStreamInterface> createMediaStream(const std::string& stream_id);
    };
}
#endif // WEBRTCFACTORY_H
