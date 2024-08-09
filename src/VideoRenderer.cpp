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
		//rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = frame.video_frame_buffer()->ToI420();
		//QImage image(buffer->DataY(), buffer->width(), buffer->height(), QImage::Format_Grayscale8);

		//// 将视频帧转换为RGB格式以显示
		//image = image.convertToFormat(QImage::Format_RGB888);

		//{
		//	QMutexLocker locker(&_mutex);
		//	_current_image = image;
		//}

		//update(); // 触发重绘
	}

	void VideoRenderer::paintEvent(QPaintEvent* event)
	{
		QMutexLocker locker(&_mutex);
		if (!_current_image.isNull())
		{
			QPainter painter(this);
			painter.drawImage(this->rect(), _current_image);
		}
	}
}