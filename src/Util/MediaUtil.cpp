#include "stdafx.h"
#include "MediaUtil.h"

namespace SoLive::Util
{
    void MediaUtil::saveAsWav(const QByteArray& audioData, const QString& filePath, int sampleRate, int channels, int bitsPerSample)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly))
        {
            LOG(Error, "Failed to create WAV file:" + std::string(filePath.toUtf8().constData()))
            return;
        }
        WavHeader header;
        header.fileSize = 36 + audioData.size();
        header.numChannels = channels;
        header.sampleRate = sampleRate;
        header.bitsPerSample = bitsPerSample;
        header.byteRate = sampleRate * channels * (bitsPerSample / 8);
        header.blockAlign = channels * (bitsPerSample / 8);
        header.dataChunkSize = audioData.size();

        // Write the WAV header
        file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));

        // Write the audio data
        file.write(audioData);

        file.close();
        LOG(Info, "WAV file saved successfully:" + std::string(filePath.toUtf8().constData()))
    }

    bool MediaUtil::convertPcmToWav(const QString& pcmFilePath, const QString& wavFilePath, int sampleRate, int channels, int bitsPerSample)
    {
        QFile pcmFile(pcmFilePath);
        QFile wavFile(wavFilePath);

        if (!pcmFile.open(QIODevice::ReadOnly))
        {
            LOG(Error,"Failed to open PCM file:"+std::string(pcmFilePath.toUtf8().constData()))
            return false;
        }

        if (!wavFile.open(QIODevice::WriteOnly))
        {
            LOG(Error,"Failed to open WAV file:" + std::string(wavFilePath.toUtf8().constData()))
            return false;
        }

        // Prepare WAV header
        WavHeader header;
        header.numChannels = channels;
        header.sampleRate = sampleRate;
        header.bitsPerSample = bitsPerSample;
        header.byteRate = sampleRate * channels * (bitsPerSample / 8);
        header.blockAlign = channels * (bitsPerSample / 8);

        // Temporary buffer for reading PCM data
        const int bufferSize = 4096;
        QByteArray buffer(bufferSize, 0);

        // Write header with placeholder sizes
        wavFile.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));

        // Write PCM data and update header sizes
        qint64 totalDataSize = 0;
        while (!pcmFile.atEnd())
        {
            qint64 bytesRead = pcmFile.read(buffer.data(), bufferSize);
            if (bytesRead > 0)
            {
                wavFile.write(buffer.data(), bytesRead);
                totalDataSize += bytesRead;
            }
        }

        // Update the WAV header with correct sizes
        header.fileSize = 36 + totalDataSize;
        header.dataChunkSize = totalDataSize;

        wavFile.seek(0); // Go back to the start of the file
        wavFile.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));

        pcmFile.close();
        wavFile.close();
        LOG(Info, "WAV file saved successfully:" + std::string(wavFilePath.toUtf8().constData()))
        return true;
    }

    QString MediaUtil::getFormatTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t time_now = std::chrono::system_clock::to_time_t(now);
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        std::tm tm_now;
        localtime_s(&tm_now, &time_now);
        std::ostringstream oss;
        oss << std::put_time(&tm_now, "%Y%m%d_%H%M%S");
        return QString::fromUtf8(oss.str().c_str());
    }
}
