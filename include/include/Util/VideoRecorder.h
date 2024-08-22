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
    #include <libavutil/imgutils.h>
    #include <libavutil/frame.h>
}
namespace SoLive::Util
{
    class VideoRecorder :public QObject
    {
        Q_OBJECT
    public:
        VideoRecorder();
        ~VideoRecorder();
        void init();
        void end();
    private Q_SLOTS:
        void start(const QString& filePath = "");
        void stop();
        void setVideoInfo(int width,int height,int bitRate,int fps);
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
        AVPacket* _packet;
        SwsContext* _swsContext;
        int _frameCount;
        QFile _videoFile;
        int _bitRate;
        int _width;
        int _height;
        int _fps;
    };
}
#endif // !VIDEORECORDER_H
