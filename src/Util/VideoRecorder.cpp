#include "stdafx.h"
#include "VideoRecorder.h"
#include "MediaManager.h"
#include "ThreadPool.h"
#include "MediaUtil.h"
#include "ConfigManager.h"
#include "MessageManager.h"

using namespace SoLive::Util;

VideoRecorder::~VideoRecorder()
{
    av_write_trailer(_formatContext);
    avcodec_close(_codecContext);
    avcodec_free_context(&_codecContext);
    avformat_free_context(_formatContext);
    av_frame_free(&_frame);
    //sws_freeContext(_swsContext);
}

void VideoRecorder::writeFrame(const webrtc::VideoFrame& frame)
{
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = frame.video_frame_buffer()->ToI420();
    if (!buffer)
    {
        return;
    }
    // Convert I420 to YUV420P
    const uint8_t* yData = buffer->DataY();
    int yStride = buffer->StrideY();

    const uint8_t* uData = buffer->DataU();
    int uStride = buffer->StrideU();

    const uint8_t* vData = buffer->DataV();
    int vStride = buffer->StrideV();

    // Fill AVFrame
    _frame->data[0] = const_cast<uint8_t*>(yData);
    _frame->data[1] = const_cast<uint8_t*>(uData);
    _frame->data[2] = const_cast<uint8_t*>(vData);

    _frame->linesize[0] = yStride;
    _frame->linesize[1] = uStride;
    _frame->linesize[2] = vStride;

    _frame->pts = _frameCount++;

    av_init_packet(&_packet);
    _packet.data = nullptr;
    _packet.size = 0;

    // Send frame to encoder
    if (avcodec_send_frame(_codecContext, _frame) < 0)
    {
        throw std::runtime_error("Error sending frame to encoder");
    }

    // Receive packet from encoder
    while (avcodec_receive_packet(_codecContext, &_packet) == 0)
    {
        _packet.pts = _packet.dts = av_rescale_q(_packet.pts, _codecContext->time_base, _videoStream->time_base);
        _packet.duration = av_rescale_q(_packet.duration, _codecContext->time_base, _videoStream->time_base);
        av_write_frame(_formatContext, &_packet);
        av_packet_unref(&_packet);
    }
}

void VideoRecorder::init()
{
	setupConnection();
	auto& thrPool = SoLive::Util::ThreadPool::instance();
	thrPool.enqueue([this]() { handleScreenShot(); });
    thrPool.enqueue([this]() { handleRecord(); });
}

void VideoRecorder::start(const QString& filePath)
{
    auto strFilePath = std::string(filePath.toUtf8().constData())+".mp4";
    _videoFile.setFileName(QString::fromUtf8(strFilePath.c_str()));
    if (!_videoFile.open(QIODevice::WriteOnly))
    {
        LOG(Info, "Failed to open or create file: " + filePath.toStdString())
            return;
    }
    _videoFile.close();

    int ret = avformat_alloc_output_context2(&_formatContext, nullptr, nullptr, strFilePath.c_str());

    if (ret < 0 || !_formatContext)
    {
        char errBuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errBuf, sizeof(errBuf));
        qDebug() << "Could not create output context:" << errBuf;
        throw std::runtime_error("Could not create output context");
    }

    _videoStream = avformat_new_stream(_formatContext, nullptr);
    if (!_videoStream)
    {
        throw std::runtime_error("Could not create new stream");
    }
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    _codecContext = avcodec_alloc_context3(codec);
    if (!_codecContext)
    {
        throw std::runtime_error("Could not allocate video codec context");
    }

    _codecContext->codec_id = AV_CODEC_ID_H264;
    _codecContext->bit_rate = 400000;
    _codecContext->width = _width;
    _codecContext->height = _height;
    _codecContext->time_base = { 1, 25 };
    _codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(_codecContext, codec, nullptr) < 0)
    {
        throw std::runtime_error("Could not open codec");
    }

    avcodec_parameters_from_context(_videoStream->codecpar, _codecContext);
    avio_open(&_formatContext->pb, strFilePath.c_str(), AVIO_FLAG_WRITE);
    avformat_write_header(_formatContext, nullptr);

    _frame = av_frame_alloc();
    _frame->format = _codecContext->pix_fmt;
    _frame->width = _codecContext->width;
    _frame->height = _codecContext->height;
    av_frame_get_buffer(_frame, 32);

    //_swsContext = sws_getContext(_width, _height, AV_PIX_FMT_YUV420P,
    //    _width, _height, AV_PIX_FMT_YUV420P,
    //    SWS_BILINEAR, nullptr, nullptr, nullptr);



    _isRecording = true;
}

void VideoRecorder::stop()
{
    _isRecording = false;
}

void VideoRecorder::setupConnection()
{
    auto& mediaManager = MediaManager::instance();
    connect(&mediaManager,
        SIGNAL(startRecordVideo(const QString&)),
        this,
        SLOT(start(const QString&)));
    connect(&mediaManager,
        SIGNAL(stopRecordVideo()),
        this,
        SLOT(stop()));
}

void VideoRecorder::handleScreenShot()
{
    while (true)
    {
        QImage image;
        auto& mediaMgr = SoLive::Util::MediaManager::instance();
        auto& videoQueue = mediaMgr.videoQueue();
        videoQueue.wait_and_pop(image);

        if (!image.isNull())
        {
            QString fileName = mediaMgr.currRoom()+"_"+SoLive::Util::MediaUtil::getFormatTime() + ".png";
            auto& cfgMgr = SoLive::Config::ConfigManager::instance();
            auto path = std::any_cast<std::string>( cfgMgr.getValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH));
            QString fullPath = QString::fromUtf8((path+"/").c_str()) + fileName;
            bool ret=image.save(fullPath);
            MSG_PUSH(ret?"截屏成功，文件保存于 "+ path :"截屏失败")
        }
    }
}

void VideoRecorder::handleRecord()
{
    while (true)
    {
        webrtc::VideoFrame* frame = nullptr;
        auto& mediaMgr = SoLive::Util::MediaManager::instance();
        auto& videoFrameQueue = mediaMgr.videoFrameQueue();
        videoFrameQueue.wait_and_pop(*frame);

        if (_isRecording||!frame)
        {
            writeFrame(*frame);
        }
    }
}