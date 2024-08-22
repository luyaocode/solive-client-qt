#include "stdafx.h"
#include "VideoRecorder.h"
#include "MediaManager.h"
#include "ThreadPool.h"
#include "MediaUtil.h"
#include "ConfigManager.h"
#include "MessageManager.h"

using namespace SoLive::Util;
VideoRecorder::VideoRecorder() :
    _isRecording(false), _formatContext(nullptr), _videoStream(nullptr),
    _codecContext(nullptr),
    _swsContext(nullptr),
    _frame(nullptr),
    _packet(nullptr),
    _frameCount(0),
    _bitRate(400000),
    _width(640),
    _height(480),
    _fps(30)
{
}

VideoRecorder::~VideoRecorder()
{
    av_write_trailer(_formatContext);
    avcodec_close(_codecContext);
    avcodec_free_context(&_codecContext);
    avformat_free_context(_formatContext);
    av_frame_free(&_frame);
    av_packet_free(&_packet);
    sws_freeContext(_swsContext);
    _frameCount = 0;
}

void VideoRecorder::writeFrame(const webrtc::VideoFrame& frame)
{
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer = frame.video_frame_buffer()->ToI420();
    if (!buffer)
    {
        return;
    }

    int width = buffer->width();
    int height = buffer->height();

    const uint8_t* yData = buffer->DataY();
    int yStride = buffer->StrideY();

    const uint8_t* uData = buffer->DataU();
    int uStride = buffer->StrideU();

    const uint8_t* vData = buffer->DataV();
    int vStride = buffer->StrideV();

    _frame->data[0] = const_cast<uint8_t*>(yData);
    _frame->data[1] = const_cast<uint8_t*>(uData);
    _frame->data[2] = const_cast<uint8_t*>(vData);

    _frame->linesize[0] = yStride;
    _frame->linesize[1] = uStride;
    _frame->linesize[2] = vStride;

    // 计算当前帧的时间戳
    // 时间基
    //AVRational time_base = _codecContext->time_base;
    //int64_t frameNumber = _frameCount++;
    //int64_t pts = (frameNumber * time_base.den) / time_base.num; // 将时间基单位转换为实际时间戳
    _frame->pts = _frameCount++;

    if (avcodec_send_frame(_codecContext, _frame) < 0)
    {
        throw std::runtime_error("Error sending frame to encoder");
    }
    while (avcodec_receive_packet(_codecContext, _packet)==0)
    {
        _packet->pts = av_rescale_q(_packet->pts, _codecContext->time_base, _videoStream->time_base);
        _packet->dts = av_rescale_q(_packet->dts, _codecContext->time_base, _videoStream->time_base);
        _packet->duration = av_rescale_q(_packet->duration, _codecContext->time_base, _videoStream->time_base);
        if (_packet->dts > _packet->pts)
        {
            throw std::runtime_error("DTS should not be greater than PTS.");
        }
        if (av_write_frame(_formatContext, _packet) < 0)
        {
            throw std::runtime_error("Error writing frame");
        }
    }
    av_packet_unref(_packet);
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

    // 1. Allocate and initialize the output format context
    avformat_alloc_output_context2(&_formatContext, nullptr, nullptr, strFilePath.c_str());
    if (!_formatContext)
    {
        throw std::runtime_error("Could not create output context.");
    }

    // 2. Create a new video stream
    _videoStream = avformat_new_stream(_formatContext, nullptr);
    auto timebase = _videoStream->time_base;
    if (!_videoStream)
    {
        throw std::runtime_error("Failed to create new stream.");
    }

    // 3. Find and initialize the codec
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        throw std::runtime_error("Codec not found.");
    }

    _codecContext = avcodec_alloc_context3(codec);
    if (!_codecContext)
    {
        throw std::runtime_error("Could not allocate video codec context.");
    }

    _codecContext->bit_rate = _bitRate;
    _codecContext->width = _width;             // Set to the desired width
    _codecContext->height = _height;            // Set to the desired height
    _codecContext->time_base = { 1, 30 };   // Assume 30 fps
    _codecContext->framerate = { 30, 1 };
    //_codecContext->gop_size = 5;           // 10s一个关键帧
    _codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    // Copy codec context parameters to the stream
    if (avcodec_open2(_codecContext, codec, nullptr) < 0)
    {
        throw std::runtime_error("Could not open codec.");
    }
    avcodec_parameters_from_context(_videoStream->codecpar, _codecContext);

    // 4. Open the output file
    if (avio_open(&_formatContext->pb, strFilePath.c_str(), AVIO_FLAG_WRITE) < 0)
    {
        throw std::runtime_error("Could not open output file.");
    }

    // 5. Write the file header
    if (avformat_write_header(_formatContext, nullptr) < 0)
    {
        throw std::runtime_error("Could not write header.");
    }

    // 6. Allocate frame and buffer
    _frame = av_frame_alloc();
    if (!_frame)
    {
        throw std::runtime_error("Could not allocate video frame.");
    }

    _frame->format = _codecContext->pix_fmt;
    _frame->width = _codecContext->width;
    _frame->height = _codecContext->height;

    int ret = av_image_alloc(_frame->data, _frame->linesize, _frame->width, _frame->height, (AVPixelFormat)_frame->format, 32);
    if (ret < 0)
    {
        throw std::runtime_error("Could not allocate raw picture buffer.");
    }

    // Initialize packet
    _packet = av_packet_alloc();
    _packet->data = nullptr;
    _packet->size = 0;

    _isRecording = true;
    _frameCount = 0;
}

void VideoRecorder::end()
{
    // 写入尾部信息
    if (_formatContext)
    {
        av_write_trailer(_formatContext);
    }

    // 关闭和释放编码上下文
    if (_codecContext)
    {
        avcodec_close(_codecContext);
        avcodec_free_context(&_codecContext);
        _codecContext = nullptr;
    }

    // 关闭输出文件
    if (_formatContext)
    {
        if (!(_formatContext->oformat->flags & AVFMT_NOFILE))
        {
            avio_closep(&_formatContext->pb);
        }
        avformat_free_context(_formatContext);
        _formatContext = nullptr;
    }
    if (_frame)
    {
        av_frame_free(&_frame);
        _frame = nullptr;
    }
    if (_packet->data)
    {
        av_packet_free(&_packet);
        _packet = nullptr;
    }

    // 重置帧计数
    _frameCount = 0;
}

void VideoRecorder::stop()
{
    _isRecording = false;
    auto& cfgMgr = SoLive::Config::ConfigManager::instance();
    auto recordPath = std::any_cast<std::string>(cfgMgr.getValue(SoLive::Config::CONFIG_RECORD_SAVE_PATH));
    MSG_PUSH("视频录制结束，文件保存于 " + recordPath)
    end();
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
    auto videoRenderer = mediaManager.videoRendererPtr();
    connect(videoRenderer.get(),
        SIGNAL(sendVideoInfo(int,int,int,int)),
        this,
        SLOT(getVideoInfo(int,int,int,int)));
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
        // 创建一个 I420Buffer
        rtc::scoped_refptr<webrtc::I420Buffer> buffer = webrtc::I420Buffer::Create(640, 480);
        int64_t timestamp_us = rtc::TimeMicros();
        // 创建一个 webrtc::VideoFrame 对象
        webrtc::VideoFrame frame = webrtc::VideoFrame::Builder()
            .set_video_frame_buffer(buffer)
            .set_timestamp_us(timestamp_us)
            .set_rotation(webrtc::kVideoRotation_0)
            .build();
        auto& mediaMgr = SoLive::Util::MediaManager::instance();
        auto& videoFrameQueue = mediaMgr.videoFrameQueue();
        videoFrameQueue.wait_and_pop(frame);
        if (_isRecording)
        {
            writeFrame(frame);
        }
    }
}

void VideoRecorder::setVideoInfo(int width, int height, int bitRate,int fps)
{
    _width = width;
    _height = height;
    _bitRate = bitRate;
    _fps = fps;
}