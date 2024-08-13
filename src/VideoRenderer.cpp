#include "VideoRenderer.h"

namespace SoLive::Ctrl
{
	VideoRenderer::VideoRenderer(QWidget* parent) :
		QWidget(parent)
	{
	}

	VideoRenderer::~VideoRenderer()
	{
	}

	void VideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
	{
        // 假设视频帧格式是I420（YUV格式）
        rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = frame.video_frame_buffer()->ToI420();
        if (!buffer)
            return;

        // 将视频帧转换为QImage（假设你想在QWidget中渲染）
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
        {
            QMutexLocker locker(&_mutex);
            _current_image = image;
        }
        update(); // 触发重绘
	}

	void VideoRenderer::paintEvent(QPaintEvent* event)
	{
        QMutexLocker locker(&_mutex);
        if (!_current_image.isNull())
        {
            QPainter painter(this);
            painter.drawImage(rect(), _current_image);
        }
	}
}