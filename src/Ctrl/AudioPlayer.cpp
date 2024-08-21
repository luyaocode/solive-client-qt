#include "stdafx.h"
#include "AudioPlayer.h"
#include "MediaManager.h"

namespace SoLive::Ctrl
{
    AudioPlayer::AudioPlayer(QObject* parent) :
        QObject(parent),
        _isRecording(false),
        _isPlaying(false)
    {

    }

    AudioPlayer::~AudioPlayer()
    {

    }

    AudioPlayer& AudioPlayer::init()
    {
        setupConnection();
        return *this;
    }

    void AudioPlayer::stop()
    {
        _isPlaying = false;
    }

    void AudioPlayer::start()
    {
        _isPlaying = true;
    }

    void AudioPlayer::OnData(const void* audio_data,
        int bits_per_sample,
        int sample_rate,
        size_t number_of_channels,
        size_t number_of_frames)
    {
        if (_isRecording)
        {
            auto& mediaManager = SoLive::Util::MediaManager::instance();
            auto& audioQueue = mediaManager.audioQueue();
            // Assuming audio data is in PCM format; adjust as needed
            QByteArray byteData(reinterpret_cast<const char*>(audio_data),
                number_of_frames * number_of_channels * (bits_per_sample / 8));
            //_audioBuffer->write(byteData);
            audioQueue.push(byteData);
            sendAudioInfo(bits_per_sample, sample_rate, number_of_channels);
        }
    }

    void AudioPlayer::OnData(const void* audio_data,
        int bits_per_sample,
        int sample_rate,
        size_t number_of_channels,
        size_t number_of_frames,
        absl::optional<int64_t> absolute_capture_timestamp_ms)
    {
        if (!_isPlaying) return;
        // Use the timestamp if needed
        OnData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
    }

    void AudioPlayer::setupConnection()
    {
        auto& mediaManager = SoLive::Util::MediaManager::instance();
        connect(&mediaManager,
            SIGNAL(startRecordAudio(const QString&)),
            this,
            SLOT(startRecord()));
        connect(&mediaManager,
            SIGNAL(stopRecordAudio()),
            this,
            SLOT(stopRecord()));
    }

    void AudioPlayer::startRecord()
    {
        _isRecording = true;
    }

    void AudioPlayer::pauseRecord()
    {
        _isRecording = false;
    }

    void AudioPlayer::stopRecord()
    {
        _isRecording = false;
    }

    void AudioPlayer::printByte(const void* audio_data,
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
    }
}
