#include "donut_av_demuxer.h"
#include "log.h"
extern"C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavutil/rational.h>
#include <libavutil/mem.h>
#include <libavutil/dict.h>
#include <libavcodec/avcodec.h>
}

#include <iostream>
#include <thread>

static void printError(int err)
{
    char buffer[1024] = { 0 };
    av_strerror(err, buffer, sizeof(buffer));
    //std::cout << buffer << std::endl;
    DN_CORE_ERROR(buffer);
}

#define PRINT_ERR_P(err) if(err != 0) {printError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {printError(err);return -1;}

namespace Donut
{
    

AVFormatContext* DonutAVDemuxer::openContext(const char* url)
{
    AVFormatContext* fmt_ctx = avformat_alloc_context();
    fmt_ctx->flags |= AVFMT_FLAG_NONBLOCK;
    fmt_ctx->flags |= AVFMT_FLAG_GENPTS;

    AVDictionary* opt = nullptr, *codec_opts = nullptr;
    av_dict_set(&opt, "stimeout", "1000000", 0);

    if (!av_dict_get(opt, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE))
    {
        av_dict_set(&opt, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
        //scan_all_pmts_set = 1;
    }

    int ret = avformat_open_input(&fmt_ctx, url, nullptr, &opt);
    PRINT_ERR_P(ret);

    av_format_inject_global_side_data(fmt_ctx);

    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    PRINT_ERR_P(ret);

    av_dump_format(fmt_ctx, 0, url, 0);

    total_duration_ = fmt_ctx->duration;
    if (total_duration_ == AV_NOPTS_VALUE)
    {
        total_duration_ = 0;
    }

    for (auto i = 0; i < fmt_ctx->nb_streams; i++)
    {
        AVStream* stream = nullptr;
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            stream = fmt_ctx->streams[i];
            video_timebase_ = stream->time_base;
            if (stream->duration == AV_NOPTS_VALUE)
            {
                AVPacket* packet = av_packet_alloc();
                while (av_read_frame(fmt_ctx, packet) >= 0)
                {
                    if (packet->stream_index == i)
                    {
                        int64_t pts = packet->pts != AV_NOPTS_VALUE ? packet->pts : packet->dts;
                        if (pts != AV_NOPTS_VALUE)
                        {
                            video_duration_ = video_duration_ > pts ? video_duration_ : pts;
                        }
                    }
                    av_packet_unref(packet);
                }
                avformat_seek_file(fmt_ctx, i, 0, 0, 0, 0);
                fmt_ctx->streams[i]->duration = video_duration_;
            }
            video_duration_ = fmt_ctx->streams[i]->duration;
            video_streams_[vs_count_] = stream;
            vs_count_++;
        }
        else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            stream = fmt_ctx->streams[i];
            if (stream->duration == AV_NOPTS_VALUE)
            {
                AVPacket* packet = av_packet_alloc();
                while (av_read_frame(fmt_ctx, packet) >= 0)
                {
                    if (packet->stream_index == i)
                    {
                        int64_t pts = packet->pts != AV_NOPTS_VALUE ? packet->pts : packet->dts;
                        if (pts != AV_NOPTS_VALUE)
                        {
                            audio_duration_ = audio_duration_ > pts ? audio_duration_ : pts;
                        }
                    }
                    av_packet_unref(packet);
                }
                avformat_seek_file(fmt_ctx, i, 0, 0, 0, 0);
                fmt_ctx->streams[i]->duration = audio_duration_;
                //fmt_ctx->streams[i]->time_base = { 1, stream->codecpar->sample_rate };
            }
            audio_timebase_ = fmt_ctx->streams[i]->time_base;
            audio_duration_ = fmt_ctx->streams[i]->duration;
            audio_streams_[as_count_] = fmt_ctx->streams[i];
            as_count_++;
        }
        else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
        {

        }

        total_duration_ = FFMAX(FFMAX(video_duration_, audio_duration_), total_duration_);
    }

    //AVPacket* packet = av_packet_alloc();
    //while (av_read_frame(fmt_ctx, packet) >= 0)
    //{
    //    DN_CORE_ERROR("read index : {}", packet->stream_index);
    //    av_packet_unref(packet);
    //}
    //avformat_seek_file(fmt_ctx, -1, 0, 0, 0, 0);

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
    int ret = 0;

    ret = av_seek_frame(fmt_ctx_, stream_index, pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);

    return ret;
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

int64_t DonutAVDemuxer::getTotalDuration(int64_t& total, int64_t& audio, int64_t& video)
{
    if (!fmt_ctx_)
    {
        return -1;
    }
    //total = fmt_ctx_->duration / 1000000LL;
    //audio = fmt_ctx_->streams[audio_index_]->duration / 1000000LL;
    //video = fmt_ctx_->streams[video_index_]->duration / 1000000LL;

    //total = fmt_ctx_->duration / 1000000LL;
    //audio = fmt_ctx_->streams[audio_index_]->duration;
    //video = fmt_ctx_->streams[video_index_]->duration;

    total = total_duration_ / 1000000LL;
    audio = audio_duration_;
    video = video_duration_;
    return total;
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
        //ret = av_seek_frame(fmt_ctx_, audio_index_, target, flags);
        ret = avformat_seek_file(fmt_ctx_, -1, min, target, max, flags);
        if (ret < 0)
        {
            DN_CORE_WARN("{0} : error while seeking", fmt_ctx_->url);
        }
    }

    return ret;
}

void DonutAVDemuxer::setPaused(bool paused)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (!fmt_ctx_) return;
    if (paused)
    {
        is_paused_ = true;
        av_read_pause(fmt_ctx_);
    }
    else
    {
        is_paused_ = false;
        av_read_play(fmt_ctx_);
    }
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