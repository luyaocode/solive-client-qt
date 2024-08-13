#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QAudioOutput>
#include <QBuffer>
#include <api/media_stream_interface.h>

class AudioPlayer : public webrtc::AudioTrackSinkInterface
{
public:
    explicit AudioPlayer(/*QAudioOutput* audioOutput*/);
    ~AudioPlayer() override;

    // Override the OnData method to handle audio data
    void OnData(const void* audio_data,
        int bits_per_sample,
        int sample_rate,
        size_t number_of_channels,
        size_t number_of_frames) override;

    // Optionally override the other OnData method if you need timestamp info
    void OnData(const void* audio_data,
        int bits_per_sample,
        int sample_rate,
        size_t number_of_channels,
        size_t number_of_frames,
        absl::optional<int64_t> absolute_capture_timestamp_ms) override;

    //QBuffer* audioBuffer() const;

private:
    //QAudioOutput* _audioOutput;
    //QBuffer* _audioBuffer;
};

#endif // AUDIOPLAYER_H
