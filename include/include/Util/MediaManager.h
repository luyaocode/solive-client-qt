#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include "ThreadSafeQueue.h"
#include <vector>
#include <atomic>
#include <mutex>
#include <QByteArray>
#include <QString>
#include "AudioRecorder.h"
#include "VideoRecorder.h"
#include "AudioPlayer.h"
#include "VideoRenderer.h"
#include "UtilDef.h"

namespace SoLive::Ctrl
{
    class VideoRender;
    class AudioPlayer;
}

namespace SoLive::Util
{
    class MediaManager:public QObject
    {
        Q_OBJECT
    public:
        static MediaManager& instance();
        void init();
        inline const QString currRoom() const { return _roomId; }
        inline bool isRecording() const { return _isRecording; }
        inline bool isPlaying() const { return _isPlaying; }
        void startRecord(RecordMode recordMode);
        void stopRecord();
        SoLive::Util::ThreadSafeQueue<QByteArray>& audioQueue() { return _audioQueue; }
        SoLive::Util::ThreadSafeQueue<QImage>& videoQueue() { return _videoQueue; }
        SoLive::Util::ThreadSafeQueue<webrtc::VideoFrame>& videoFrameQueue() { return _videoFrameQueue; }
        QScopedPointer<SoLive::Util::AudioRecorder>& audioRecorderPtr() { return _audioRecorderPtr; }
        QScopedPointer<SoLive::Util::VideoRecorder>& videoRecorderPtr() { return _videoRecorderPtr; }
        QSharedPointer<SoLive::Ctrl::VideoRenderer>& videoRendererPtr() { return _videoRendererPtr; }
        QSharedPointer<SoLive::Ctrl::AudioPlayer>& audioPlayerPtr() { return _audioPlayerPtr; }
    Q_SIGNALS:
        void startRecordAudio(const QString& filePath);
        void startRecordVideo(const QString& filePath);
        void stopRecordAudio();
        void stopRecordVideo();
        void screenShot();
        void startPlay(const QString& filePath);
        void stopPlay();
        void sendEvent(const Event& e);
    private Q_SLOTS:
        inline void onCurrRoomChanged(const QString& room) { _roomId = room; }
        void onEvent(const Event& e);
    private:
        MediaManager();
        ~MediaManager();
        MediaManager(const MediaManager&) = delete;
        MediaManager& operator=(const MediaManager&) = delete;
        void setupConnection();

    private:
        QString _roomId;
        RecordMode _recordMode{ RecordMode::VideoAndAudio };
        SoLive::Util::ThreadSafeQueue<QByteArray>   _audioQueue;        // 音频帧队列
        SoLive::Util::ThreadSafeQueue<QImage>       _videoQueue;        // 视频帧队列
        SoLive::Util::ThreadSafeQueue<webrtc::VideoFrame>       _videoFrameQueue;      // 原始视频帧队列

        QScopedPointer<SoLive::Util::AudioRecorder> _audioRecorderPtr;
        QScopedPointer<SoLive::Util::VideoRecorder> _videoRecorderPtr;

        std::atomic<bool> _isRecording{ false };
        mutable std::mutex _recordingMutex;
        std::atomic<bool> _isPlaying{ false };
        mutable std::mutex _isPlayingMutex;

        QSharedPointer<SoLive::Ctrl::VideoRenderer> _videoRendererPtr;
        QSharedPointer<SoLive::Ctrl::AudioPlayer> _audioPlayerPtr;
    };
}

#endif // MEDIAMANAGER_H
