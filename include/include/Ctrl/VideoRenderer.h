#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMutexLocker>
#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>

namespace Ui
{
    class VideoControlBar;
};

namespace SoLive::Ctrl
{
    struct Overlay
    {
        QString _text;
        QPoint _position;
        int _width;
        int _height;
        QFont _font;
        QColor _color;
        QColor _background;
        QImage _image;
        Overlay(const QString& text = QString(),
            const QPoint& position = QPoint(),
            int width = 100,
            int height = 100,
            QFont font = QFont("Arial", 16),
            const QColor& color = Qt::white,
            const QColor& background = Qt::black,
            const QImage& image = QImage())
            :
            _text(text),
            _position(position),
            _width(width),
            _height(height),
            _font(font),
            _color(color),
            _background(background),
            _image(image)
        {}
    };

    class VideoRenderer : public QWidget,
        public rtc::VideoSinkInterface<webrtc::VideoFrame>
    {
        Q_OBJECT
    public:
        virtual ~VideoRenderer();
        VideoRenderer(QWidget* parent = nullptr);
        void init();
        void clear();
        void start();
        inline bool isRendering() const { return _isRendering; }
        void OnFrame(const webrtc::VideoFrame& frame) override;
        void addOverlay(const Overlay& overlay);
    protected:
        void paintEvent(QPaintEvent* event) override;
    private Q_SLOTS:
        void onRoomConnected(const QString& roomId);
        void onScreenShot() { _screenShotRequest = true; }
        void startRecord();
        void pauseRecord();
        void stopRecord();
    private:
        Ui::VideoControlBar*    _videoCtrlBar;
        QImage                  _current_image;
        QMutex                  _mutex;
        bool                    _clearRequested;
        std::vector<Overlay>    _overlays;
        bool                    _isRendering;
        bool                    _screenShotRequest;
        bool                    _isRecording;
    private:
        void setupUi();
        void setupConnection();
        void drawFrame();
        void drawOverlay();
    };
}
#endif // VIDEORENDERER_H

