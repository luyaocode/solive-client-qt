#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <thread>
#include <mutex>
#include <QString>
#include <QObject>
#include <QFile>

namespace SoLive::Util
{
    class AudioRecorder:public QObject
    {
        Q_OBJECT
    public:
        AudioRecorder();
        void init();
    private:
        void setupConnection();
        void recordThread();
        QFile _audioFile;
        std::mutex _fileMutex;
        std::atomic<bool> _isRecording;
        std::thread _recordingThread;
        int _bits_per_sample;
        int _sample_rate;
        size_t _number_of_channels;
    private Q_SLOTS:
        void start(const QString& filePath);
        void stop();
        void setAudioInfo(int bits_per_sample, int sample_rate, int number_of_channels);
    };
    
}
#endif // AUDIORECORDER_H
