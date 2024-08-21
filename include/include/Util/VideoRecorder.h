#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QObject>
#include <QString>
#include <mutex>
#include <condition_variable>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}
namespace SoLive::Util
{
    class VideoRecorder :public QObject
    {
        Q_OBJECT
    public:
        VideoRecorder()
            : _isRecording(false), _formatContext(nullptr), _videoStream(nullptr), _codecContext(nullptr), _frame(nullptr),
            _width(800), _height(600), _frameCount(0)
        {
        }
        ~VideoRecorder();
        void init();
    private Q_SLOTS:
        void start(const QString& filePath = "");
        void stop();
    private:
        void setupConnection();
        void handleScreenShot();
        void handleRecord();
        void writeFrame(const webrtc::VideoFrame& frame);
    private:
        bool _isRecording;
        AVFormatContext* _formatContext;
        AVStream* _videoStream;
        AVCodecContext* _codecContext;
        AVFrame* _frame;
        AVPacket _packet;
        SwsContext* _swsContext;
        int _frameCount;
        QFile _videoFile;
        int _width;
        int _height;
    };
}
#endif // !VIDEORECORDER_H
