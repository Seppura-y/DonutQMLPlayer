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

    for (auto i = 0; i < fmt_ctx->nb_streams; i++)
    {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_streams_[vs_count_] = fmt_ctx->streams[i];
            vs_count_++;
        }
        else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_streams_[as_count_] = fmt_ctx->streams[i];
            as_count_++;
        }
        else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
        {

        }
    }

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

    return 0;
}

int64_t DonutAVDemuxer::getTotalDuration()
{
    //std::unique_lock<std::mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return -1;
    }

    int tns, thh, tmm, tss;

    int64_t total_duration = fmt_ctx_->duration / 1000000LL;
    thh = total_duration / 3600;
    tmm = (total_duration % 3600) / 60;
    tss = (total_duration % 60);

    return total_duration;
}

AVStream* DonutAVDemuxer::getVideoStream(int index)
{
    int i = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_VIDEO, index, -1, NULL, 0);
    AVStream* stream = fmt_ctx_->streams[i];
    return nullptr;
}

AVStream* DonutAVDemuxer::getAudioStream(int index)
{
    return nullptr;
}

int64_t DonutAVDemuxer::getStartTime()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (fmt_ctx_)
    {
        return fmt_ctx_->start_time;
    }
    else
    {
        return 0;
    }
}

int DonutAVDemuxer::seekFile(int64_t min, int64_t target, int64_t max, int flags)
{
    std::lock_guard<std::mutex> lock(mtx_);
    int ret = 0;
    if (fmt_ctx_)
    {
        ret = avformat_seek_file(fmt_ctx_, -1, min, target, max, flags);
        if (ret < 0)
        {
            DN_CORE_WARN("%s : error while seeking", fmt_ctx_->url);
        }
    }

    return ret;
}

int64_t DonutAVDemuxer::getStreamStartTime(int stream_index)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (fmt_ctx_)
    {
        return fmt_ctx_->streams[stream_index]->start_time;
    }
    else
    {
        return 0;
    }
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