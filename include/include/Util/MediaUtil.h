#ifndef MEDIAUTIL_H
#define MEDIAUTIL_H

#include <QByteArray>
#include <QString>

namespace SoLive::Util
{
    class MediaUtil
    {
    public:
        // WAV header structure
        struct WavHeader
        {
            char riff[4] = { 'R', 'I', 'F', 'F' };
            quint32 fileSize;
            char wave[4] = { 'W', 'A', 'V', 'E' };
            char fmt[4] = { 'f', 'm', 't', ' ' };
            quint32 fmtChunkSize = 16;
            quint16 audioFormat = 1; // PCM
            quint16 numChannels;
            quint32 sampleRate;
            quint32 byteRate;
            quint16 blockAlign;
            quint16 bitsPerSample;
            char data[4] = { 'd', 'a', 't', 'a' };
            quint32 dataChunkSize;
        };
        static void saveAsWav(const QByteArray& audioData, const QString& filePath, int sampleRate, int channels, int bitsPerSample);
        static bool convertPcmToWav(const QString& pcmFilePath, const QString& wavFilePath, int sampleRate, int channels, int bitsPerSample);
        static QString getFormatTime();
    };
}

#endif // MEDIAUTIL_H
