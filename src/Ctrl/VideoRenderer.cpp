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
        _isRecording(false),
        _muted(false)
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
        setIsRendering(false);
        update();
    }

    void VideoRenderer::start()
    {
        QMutexLocker locker(&_mutex);
        setIsRendering(true);
    }

    void VideoRenderer::setIsRendering(bool flag)
    {
        if (isRendering() != flag)
        {
            _isRendering = flag;
            renderStatusChanged(); //SIGNAL
        }
    }

	void VideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
	{
        if (!isRendering())
        {
            return;
        }
        // 假设视频帧格式是I420（YUV格式）
        rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = frame.video_frame_buffer()->ToI420();
        if (!buffer)
            return;

        // 将视频帧转换为QImage
        int width = buffer->width();
        int height = buffer->height();
        if (_isRecording)
        {
            auto& mediaMgr = SoLive::Util::MediaManager::instance();
            auto& videoFrameQueue = mediaMgr.videoFrameQueue();
            videoFrameQueue.push(frame);
        }
        sendVideoInfo(width, height); //SIGNAL

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
        connect(&mediaMgr, SIGNAL(startRecordVideo(const QString&)), this, SLOT(onStartRecord()));
        connect(&mediaMgr, SIGNAL(stopRecordVideo()), this, SLOT(onStopRecord()));
        connect(&mediaMgr, SIGNAL(sendEvent(const Event&)), this, SLOT(onEvent(const Event&)));

        connect(this, SIGNAL(renderStatusChanged()), this, SLOT(onRenderStausChanged()));
        connect(_videoCtrlBar->play_pause_btn, &QPushButton::clicked, [this]()
            {
                Event e;
                e.type = EventType::PlayStatus;
                e.play = !isRendering();
                sendEvent(e);
            });
        connect(_videoCtrlBar->mute_btn, &QPushButton::clicked, [this]()
            {
                Event e;
                e.type = EventType::Muted;
                e.muted = !_muted;
                sendEvent(e);
            });

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

    void VideoRenderer::onStartRecord()
    {
        _isRecording = true;
    }

    void VideoRenderer::onPauseRecord()
    {
        _isRecording = false;
    }

    void VideoRenderer::onStopRecord()
    {
        _isRecording = false;
    }

    void VideoRenderer::onRenderStausChanged()
    {
        if(isRendering())
        {
            _videoCtrlBar->play_pause_btn->setText("暂停");
        }
        else
        {
            _videoCtrlBar->play_pause_btn->setText("播放");
            _muted = false;
            _videoCtrlBar->mute_btn->setText(tr("静音"));
        }
        _videoCtrlBar->mute_btn->setEnabled(isRendering());
    }

    void VideoRenderer::onEvent(const Event& e)
    {
        switch (e.type)
        {
        case EventType::PlayStatus:
            setIsRendering(e.play);
            break;
        case EventType::Muted:
            _muted = e.muted;
            _videoCtrlBar->mute_btn->setText(_muted?tr("取消静音"):tr("静音"));
            break;
        default:
            break;
        }
    }
}