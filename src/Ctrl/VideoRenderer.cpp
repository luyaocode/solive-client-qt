#include "stdafx.h"
#include "ui_VideoControlBar.h"
#include "VideoRenderer.h"
#include "MediaManager.h"
#include "MediaUtil.h"

namespace SoLive::Ctrl
{
    VideoRenderer::VideoRenderer(QWidget* parent) :
        QWidget(parent),
        _videoCtrlBar(new Ui::VideoControlBar),
        _clearRequested(false),
        _isRendering(false),
        _screenShotRequest(false),
        _isRecording(false)
	{
        setupUi();
	}

	VideoRenderer::~VideoRenderer()
	{
        delete _videoCtrlBar;
	}

    void VideoRenderer::init()
    {
        setupConnection();
    }

    void VideoRenderer::setupUi()
    {
        auto container = new QWidget(this);
        _videoCtrlBar->setupUi(container);
        auto mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch();
        mainLayout->addWidget(container);
        mainLayout->setContentsMargins(0,0,0,0);
        setLayout(mainLayout);
    }

    void VideoRenderer::clear()
    {
        QMutexLocker locker(&_mutex);
        _clearRequested = true;
        _current_image = QImage();
        _overlays.clear();
        _isRendering = false;
        update();
    }

    void VideoRenderer::start()
    {
        QMutexLocker locker(&_mutex);
        _isRendering = true;
    }

	void VideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
	{
        if (!_isRendering)
        {
            return;
        }
        if (_isRecording)
        {
            auto& mediaMgr = SoLive::Util::MediaManager::instance();
            auto& videoFrameQueue = mediaMgr.videoFrameQueue();
            videoFrameQueue.push(frame);
        }
        // 假设视频帧格式是I420（YUV格式）
        rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = frame.video_frame_buffer()->ToI420();
        if (!buffer)
            return;

        // 将视频帧转换为QImage
        int width = buffer->width();
        int height = buffer->height();

        QImage image(width, height, QImage::Format_RGB32);

        // 填充 QImage 数据
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int Y = buffer->DataY()[y * buffer->StrideY() + x];
                int U = buffer->DataU()[(y / 2) * buffer->StrideU() + (x / 2)];
                int V = buffer->DataV()[(y / 2) * buffer->StrideV() + (x / 2)];

                int R = Y + 1.402 * (V - 128);
                int G = Y - 0.344136 * (U - 128) - 0.714136 * (V - 128);
                int B = Y + 1.772 * (U - 128);

                image.setPixel(x, y, qRgb(R, G, B));
            }
        }

        // 更新UI
        QMutexLocker locker(&_mutex);
        _current_image = image;
        update(); 

        // 处理截屏
        if (_screenShotRequest)
        {
            auto& mediaMgr = SoLive::Util::MediaManager::instance();
            auto& videoQueue = mediaMgr.videoQueue();
            videoQueue.push(image);
            _screenShotRequest = false;
        }
	}

	void VideoRenderer::paintEvent(QPaintEvent* event)
	{
        QMutexLocker locker(&_mutex);
        QWidget::paintEvent(event);
        drawFrame();
        drawOverlay();
	}

    void VideoRenderer::onRoomConnected(const QString& roomId)
    {
        QFontDatabase fontDatabase;
        QStringList fontFamilies = fontDatabase.families();
        if (fontFamilies.empty())
        {
            throw std::runtime_error("No fonts available on the system.");
        }
        addOverlay(Overlay(roomId, {10,10},140,60,QFont(fontFamilies[0],12)));
    }


    void VideoRenderer::setupConnection()
    {
        auto& mediaMgr = SoLive::Util::MediaManager::instance();
        connect(&mediaMgr,SIGNAL(screenShot()),this,SLOT(onScreenShot()));
    }

    void VideoRenderer::addOverlay(const Overlay& overlay)
    {
        _overlays.push_back(overlay);
        update();
    }

    void VideoRenderer::drawFrame()
    {
        QPainter painter(this);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        if (_clearRequested)
        {
            painter.fillRect(rect(), Qt::black);
            _clearRequested = false;
        }
        else if (!_current_image.isNull())
        {
            painter.drawImage(rect(), _current_image);
        }
        else
        {
            painter.fillRect(rect(), Qt::black);
        }
    }

    void VideoRenderer::drawOverlay()
    {
        QPainter painter(this);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        for (const auto& overlay : _overlays)
        {
            // 绘制背景
            if (overlay._background.isValid())
            {
                painter.setBrush(overlay._background);
                painter.setPen(Qt::NoPen);
                QRect backgroundRect(overlay._position, QSize(overlay._width, overlay._height));
                painter.drawRect(backgroundRect);
            }

            // 绘制图片
            if (!overlay._image.isNull())
            {
                QRect imageRect(overlay._position, QSize(overlay._width, overlay._height));
                painter.drawImage(imageRect, overlay._image);
            }

            // 绘制文本
            painter.setFont(overlay._font);
            painter.setPen(overlay._color);
            QRect textRect(overlay._position, QSize(overlay._width, overlay._height));
            painter.drawText(textRect, Qt::AlignCenter, overlay._text);
        }
    }

    void VideoRenderer::startRecord()
    {
        _isRecording = true;
    }

    void VideoRenderer::pauseRecord()
    {
        _isRecording = false;
    }

    void VideoRenderer::stopRecord()
    {
        _isRecording = false;
    }
}