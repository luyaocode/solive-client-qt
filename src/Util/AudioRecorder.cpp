#include "stdafx.h"
#include "AudioRecorder.h"
#include "MediaManager.h"
#include "Logger.h"
#include "MediaUtil.h"
#include "MessageManager.h"
#include "ConfigManager.h"
#include "ConfigDef.h"

namespace SoLive::Util
{
    AudioRecorder::AudioRecorder() : _isRecording(false),
        _bits_per_sample(0),
        _sample_rate(0),
        _number_of_channels(0)
    {
    }

    void AudioRecorder::init()
    {
        setupConnection();
    }

    void AudioRecorder::setupConnection()
    {
        auto& mediaManager = MediaManager::instance();
        connect(&mediaManager,
            SIGNAL(startRecordAudio(const QString&)),
            this,
            SLOT(start(const QString&)));
        connect(&mediaManager,
            SIGNAL(stopRecordAudio()),
            this,
            SLOT(stop()));
        auto audioPlayerPtr = mediaManager.audioPlayerPtr();
        bool flag=connect(audioPlayerPtr.get(),
            SIGNAL(sendAudioInfo(int,int,int)),
            this,
            SLOT(setAudioInfo(int,int,int)));

    }

    void AudioRecorder::start(const QString& filePath)
    {
        QString fileName = filePath + ".pcm";
        std::lock_guard<std::mutex> lock(_fileMutex);
        if (!_isRecording)
        {
            _audioFile.setFileName(fileName);
            if (!_audioFile.open(QIODevice::WriteOnly))
            {
                LOG(Info, "Failed to open or create file: " + filePath.toStdString())
                return;
            }
            _isRecording = true;
            _recordingThread = std::thread(&AudioRecorder::recordThread, this);
        }
    }

    void AudioRecorder::stop()
    {
        std::lock_guard<std::mutex> lock(_fileMutex);
        _isRecording = false;
        if (_recordingThread.joinable())
        {
            _recordingThread.join();
        }
        if (_audioFile.isOpen())
        {
            _audioFile.close();
        }
        MSG_PUSH("音频录制结束");

        // 转换为WAV格式
        QFileInfo fileInfo(_audioFile.fileName());
        QString pcmFilePath = fileInfo.absoluteFilePath();
        QString outputWavPath = fileInfo.absolutePath() + "/" + fileInfo.baseName() + ".wav";
        bool convertResult = MediaUtil::convertPcmToWav(
            pcmFilePath,
            outputWavPath,
            _sample_rate, _number_of_channels, _bits_per_sample);
        MSG_PUSH(convertResult ? "音频转码成功" : "音频转码失败")
        auto& cfgMgr = SoLive::Config::ConfigManager::instance();
        auto recordPath = std::any_cast<std::string>(cfgMgr.getValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH));
        MSG_PUSH("音频录制结束，文件保存于 "+ recordPath)
        // 删除原始PCM文件
        if (convertResult)
        {
            QFile::remove(pcmFilePath);
        }
    }

    void AudioRecorder::setAudioInfo(int bits_per_sample, int sample_rate, int number_of_channels)
    {
        _bits_per_sample = bits_per_sample;
        _sample_rate = sample_rate;
        _number_of_channels = number_of_channels;
    }

    void AudioRecorder::recordThread()
    {
        auto& mediaManager = MediaManager::instance();
        auto& audioQueue = mediaManager.audioQueue();
        while (_isRecording || !audioQueue.isEmpty())
        {
            QByteArray data;
            audioQueue.wait_and_pop(data);
            if (_audioFile.isOpen())
            {
                _audioFile.write(data);
            }
        }
    }
}
