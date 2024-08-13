#include "AudioPlayer.h"
#include <iostream>
#include <absl/types/optional.h>

AudioPlayer::AudioPlayer(/*QAudioOutput* audioOutput*/)
    //: _audioOutput(audioOutput)
{
    //_audioBuffer = new QBuffer;
    //_audioBuffer->open(QIODevice::WriteOnly);
}

AudioPlayer::~AudioPlayer()
{
    //_audioBuffer->close();
    //delete _audioBuffer;
}

void AudioPlayer::OnData(const void* audio_data,
    int bits_per_sample,
    int sample_rate,
    size_t number_of_channels,
    size_t number_of_frames)
{
    if (!audio_data) return;

    const unsigned char* data = static_cast<const unsigned char*>(audio_data);

    std::cout << "Audio data details:" << std::endl;
    std::cout << "Bits per sample: " << bits_per_sample << std::endl;
    std::cout << "Sample rate: " << sample_rate << std::endl;
    std::cout << "Number of channels: " << number_of_channels << std::endl;
    std::cout << "Number of frames: " << number_of_frames << std::endl;

    // Print the first few bytes of audio data
    std::cout << "First few bytes of audio data: ";
    for (size_t i = 0; i < std::min<size_t>(20, number_of_frames * number_of_channels * (bits_per_sample / 8)); ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal format
    // Assuming audio data is in PCM format; adjust as needed
    QByteArray byteData(reinterpret_cast<const char*>(audio_data),
        number_of_frames * number_of_channels * (bits_per_sample / 8));
    //_audioBuffer->write(byteData);
}

void AudioPlayer::OnData(const void* audio_data,
    int bits_per_sample,
    int sample_rate,
    size_t number_of_channels,
    size_t number_of_frames,
    absl::optional<int64_t> absolute_capture_timestamp_ms)
{
    // Use the timestamp if needed
    OnData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
}

//QBuffer* AudioPlayer::audioBuffer() const
//{
//    return _audioBuffer;
//}
