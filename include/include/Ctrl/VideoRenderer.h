#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMutexLocker>
#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>
#include "UtilDef.h"
using namespace SoLive::Util;

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
    enum class ScreenSize
    {
        Origin,
        Max,
        Full
    };

    class VideoRenderer : public QWidget,
        public rtc::VideoSinkInterface<webrtc::VideoFrame>
    {
        Q_OBJECT
            Q_PROPERTY(bool isRendering READ isRendering WRITE setIsRendering NOTIFY renderStatusChanged)
            Q_PROPERTY(ScreenSize screenSize READ screenSize WRITE setScreenSize NOTIFY screenSizeChanged)


    public:
        virtual ~VideoRenderer();
        VideoRenderer(QWidget* parent = nullptr);
        void init();
        void setParent(QWidget* parent = nullptr);
        void clear();
        void start();
        inline bool isRendering() const { return _isRendering; }
        void setIsRendering(bool flag);
        void OnFrame(const webrtc::VideoFrame& frame) override;
        void addOverlay(const Overlay& overlay);
    Q_SIGNALS:
        // TODO
        void sendVideoInfo(int width, int height, int bitRate=40000,int fps=30);
        void renderStatusChanged();
        void sendEvent(const Event& e);
        void screenSizeChanged();
    protected:
        void paintEvent(QPaintEvent* event) override;
    private Q_SLOTS:
        void onRoomConnected(const QString& roomId);
        void onScreenShot() { _screenShotRequest = true; }
        void onStartRecord();
        void onPauseRecord();
        void onStopRecord();
        void onRenderStausChanged();
        void onEvent(const Event& e);
        void onCurrRoomChanged(const QString& room);
        void onScreenSizeChanged();
    private:
        Ui::VideoControlBar*    _videoCtrlBar{nullptr};
        ScreenSize              _screenSize{ScreenSize::Origin};
        double                  _ratio{1.0};
        QWidget*                _parent{nullptr};
        QWidget*                 _videoCtrContainer;
        QImage                  _current_image;
        QMutex                  _mutex;
        bool                    _clearRequested;
        std::vector<Overlay>    _overlays;
        bool                    _isRendering;
        bool                    _screenShotRequest;
        bool                    _isRecording;
        bool                    _muted;
    private:
        void setupUi();
        void setupConnection();
        void drawFrame();
        void drawOverlay();
        ScreenSize screenSize() const { return _screenSize; }
        void setScreenSize(ScreenSize ssize);
    };
}
#endif // VIDEORENDERER_H

