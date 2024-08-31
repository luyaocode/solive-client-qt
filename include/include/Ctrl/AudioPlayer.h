#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <api/media_stream_interface.h>
#include <absl/types/optional.h>
#include "UtilDef.h"
using namespace SoLive::Util;

namespace SoLive::Ctrl
{
    class AudioPlayer :public QObject, public webrtc::AudioTrackSinkInterface
    {
        Q_OBJECT
    public:
        explicit AudioPlayer(QObject* parent = nullptr);
        virtual ~AudioPlayer();
        AudioPlayer& init();
        void stop();
        void start();
        void pause();
        inline bool isPlaying() const { return _isPlaying; }

        void OnData(const void* audio_data,
            int bits_per_sample,
            int sample_rate,
            size_t number_of_channels,
            size_t number_of_frames) override;

        void OnData(const void* audio_data,
            int bits_per_sample,
            int sample_rate,
            size_t number_of_channels,
            size_t number_of_frames,
            absl::optional<int64_t> absolute_capture_timestamp_ms) override;

    private:
        void setupConnection();
        void printByte(const void* audio_data,
            int bits_per_sample,
            int sample_rate,
            size_t number_of_channels,
            size_t number_of_frames);
    private:
        bool _isRecording;
        bool _isPlaying;
        QString _roomId;
    Q_SIGNALS:
        void sendAudioInfo(int bits_per_sample,int sample_rate,int number_of_channels);
    private Q_SLOTS:
        void onStartRecord();
        void onPauseRecord();
        void onStopRecord();
        void onEvent(const Event& e);
    };
}
#endif // AUDIOPLAYER_H
