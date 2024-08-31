#include "stdafx.h"
#include "PeerConnectionFactory.h"
#include "api/peer_connection_interface.h"

#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <rtc_base/ssl_adapter.h>

#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"


namespace SoLive::PeerConnection
{
    PeerConnectionFactory& PeerConnectionFactory::getInstance()
    {
        static PeerConnectionFactory instance;
        return instance;
    }

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> PeerConnectionFactory::getFactory()
    {
        return _factory;
    }

    PeerConnectionFactory::PeerConnectionFactory()
    {
        initialize();
    }

    void PeerConnectionFactory::initialize()
    {
        _networkThread = rtc::Thread::CreateWithSocketServer();
        _workerThread = rtc::Thread::Create();
        _signalingThread = rtc::Thread::Create();

        _networkThread->SetName("network_thread", nullptr);
        _signalingThread->SetName("signaling_thread", nullptr);
        _workerThread->SetName("worker_thread", nullptr);

        if (!_networkThread->Start() || !_signalingThread->Start() || !_workerThread->Start())
        {
            throw std::runtime_error("Thread start error");
        }

        _factory = webrtc::CreatePeerConnectionFactory(
            _networkThread.get(),
            _workerThread.get(),
            _signalingThread.get(),
            nullptr /*default_adm*/,
            webrtc::CreateBuiltinAudioEncoderFactory(),
            webrtc::CreateBuiltinAudioDecoderFactory(),
            std::make_unique<webrtc::VideoEncoderFactoryTemplate<
            webrtc::LibvpxVp8EncoderTemplateAdapter,
            webrtc::LibvpxVp9EncoderTemplateAdapter,
            webrtc::OpenH264EncoderTemplateAdapter,
            webrtc::LibaomAv1EncoderTemplateAdapter>>(),
            std::make_unique<webrtc::VideoDecoderFactoryTemplate<
            webrtc::LibvpxVp8DecoderTemplateAdapter,
            webrtc::LibvpxVp9DecoderTemplateAdapter,
            webrtc::OpenH264DecoderTemplateAdapter,
            webrtc::Dav1dDecoderTemplateAdapter>>(),
            nullptr /*audio_mixer*/,
            nullptr /*audio_processing*/);
    }
}
