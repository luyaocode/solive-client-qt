#include "stdafx.h"
#include "MediaManager.h"
#include "VideoRenderer.h"
#include "AudioPlayer.h"
#include "ConfigManager.h"
#include "ConfigDef.h"
#include "MediaUtil.h"
#include "MessageManager.h"

namespace SoLive::Util
{
    MediaManager& MediaManager::instance()
    {
        static MediaManager instance;
        return instance;
    }

    MediaManager::MediaManager():_recordMode(RecordMode::VideoAndAudio)
    {
        _videoRecorderPtr.reset(new VideoRecorder);
        _audioRecorderPtr.reset(new AudioRecorder);
        _videoRendererPtr.reset(new SoLive::Ctrl::VideoRenderer());
        _audioPlayerPtr.reset(new SoLive::Ctrl::AudioPlayer());
    }


    void MediaManager::init()
    {
        _audioRecorderPtr->init();
        _videoRecorderPtr->init();
        _audioPlayerPtr->init();
        _videoRendererPtr->init();
        setupConnection();
    }

    void MediaManager::setupConnection()
    {
        connect(_videoRendererPtr.get(), SIGNAL(sendEvent(const Event&)), this, SLOT(onEvent(const Event&)));
    }

    MediaManager::~MediaManager()
    {
        stopRecord();
        stopPlay();
    }

    void MediaManager::onEvent(const Event& e)
    {
        switch (e.type)
        {
        case EventType::PlayStatus:
        case EventType::Muted:
        case EventType::Volume:
        case EventType::Reconnect:
            sendEvent(e);
            break;
        default:
            break;
        }
    }

    void MediaManager::startRecord(RecordMode recordMode)
    {
        std::lock_guard<std::mutex> lock(_recordingMutex);
        _isRecording = true;
        _recordMode = recordMode;
        auto& configManager = SoLive::Config::ConfigManager::instance();
        std::string dir = std::any_cast<std::string>(configManager.getValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH));
        QString formatTime = MediaUtil::getFormatTime();
        QString filePath = QString::fromUtf8((dir+"/").c_str()) + _roomId+ "_"+formatTime;
        switch (recordMode)
        {
        case RecordMode::VideoAndAudio:
            startRecordVideo(filePath);
            startRecordAudio(filePath);
            break;
        case RecordMode::OnlyVideo:
            startRecordVideo(filePath);
            break;
        case RecordMode::OnlyAudio:
            startRecordAudio(filePath);
            break;
        default:
            break;
        }
        MSG_PUSH("开始录制直播")
    }

    void MediaManager::stopRecord()
    {
        std::lock_guard<std::mutex> lock(_recordingMutex);
        switch (_recordMode)
        {
        case RecordMode::VideoAndAudio:
            stopRecordAudio();
            stopRecordVideo();
            break;
        case RecordMode::OnlyVideo:
            stopRecordVideo();
            break;
        case RecordMode::OnlyAudio:
            stopRecordAudio();
            break;
        default:
            break;
        }

        _isRecording = false;
    }
}