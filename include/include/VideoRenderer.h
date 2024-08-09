#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMutexLocker>
#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>

namespace SoLive::Ctrl
{
    class VideoRenderer : public QWidget,
        public rtc::VideoSinkInterface<webrtc::VideoFrame>
    {
        Q_OBJECT
    public:
        VideoRenderer();
        virtual ~VideoRenderer();
        VideoRenderer(QWidget* parent = nullptr);
        void OnFrame(const webrtc::VideoFrame& frame) override;
    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QImage _current_image;
        QMutex _mutex;
    };
}
#endif // VIDEORENDERER_H

