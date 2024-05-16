#include "donut_av_demuxer.h"
#include "log.h"
extern"C"
{
#include <libavformat/avformat.h>
}

#include <iostream>
#include <thread>

static void printError(int err)
{
    char buffer[1024] = { 0 };
    av_strerror(err, buffer, sizeof(buffer));
    std::cout << buffer << std::endl;
}

#define PRINT_ERR_P(err) if(err != 0) {printError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {printError(err);return -1;}

namespace Donut
{
    

AVFormatContext* DonutAVDemuxer::openContext(const char* url)
{
    AVFormatContext* fmt_ctx = avformat_alloc_context();
    fmt_ctx->flags |= AVFMT_FLAG_NONBLOCK;

    AVDictionary* opt = nullptr;
    av_dict_set(&opt, "stimeout", "1000000", 0);

    int ret = avformat_open_input(&fmt_ctx, url, nullptr, &opt);
    PRINT_ERR_P(ret);

    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    PRINT_ERR_P(ret);

    av_dump_format(fmt_ctx, 0, url, 0);

    av_dict_free(&opt);

    return fmt_ctx;
}

int DonutAVDemuxer::readPacket(AVPacket* pkt)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return -1;
    }

    int ret = av_read_frame(fmt_ctx_, pkt);
    PRINT_ERR_I(ret);

    last_proc_time_ = GetCurrentTimeMsec();
    return 0;
}

int DonutAVDemuxer::readPacket(std::shared_ptr<DonutAVPacket>& pkt)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return -1;
    }
    int ret = av_read_frame(fmt_ctx_, pkt->getPacket());
    if (ret != 0)
    {
        char buffer[1024] = { 0 };
        av_strerror(ret, buffer, sizeof(buffer));
        DN_CORE_WARN("av_read_frame : ", buffer);
    }
    last_proc_time_ = GetCurrentTimeMsec();
    return 0;
}

int DonutAVDemuxer::readPacket(std::shared_ptr<DonutAVPacketQueue>& pkt_queue)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return -1;
    }
    auto pkt = std::make_shared<DonutAVPacket>();
    int ret = av_read_frame(fmt_ctx_, pkt->getPacket());
    if (ret != 0)
    {
        char buffer[1024] = { 0 };
        av_strerror(ret, buffer, sizeof(buffer));
        DN_CORE_WARN("av_read_frame : ", buffer);
    }
    last_proc_time_ = GetCurrentTimeMsec();
    pkt_queue->packetQueuePut(pkt);
    return 0;
}

int DonutAVDemuxer::seekByPts(long long pts, int stream_index)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return -1;
    }
    int ret = av_seek_frame(fmt_ctx_, stream_index, pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    PRINT_ERR_I(ret);
    return 0;
}

int64_t DonutAVDemuxer::getTotalDuration()
{
    //std::unique_lock<std::mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return -1;
    }
    return fmt_ctx_->duration;
}

//DonutAVRational DonutAVDemuxer::getFrameRate()
//{
//    if (!fmt_ctx_)
//    {
//        return DonutAVRational(0, 0);
//    }
//    if (hasVideo())
//    {
//        return DonutAVRational(fmt_ctx_->streams[video_index_]->r_frame_rate.num, fmt_ctx_->streams[video_index_]->r_frame_rate.den);
//    }
//}

}