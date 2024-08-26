#include "i_donut_audio_player.h"

#include "donut_sdl_audio_player.h"

#include <iostream>
#include <SDL2/SDL.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

namespace Donut
{

void IDonutAudioPlayer::push(AVFrame* frame)
{
    if (!frame || !frame->data[0])
    {
        return;
    }

    std::vector<unsigned char> buf;
    int sample_size = av_get_bytes_per_sample((AVSampleFormat)frame->format);
    //int channels = frame->channels;

    int channels = frame->ch_layout.nb_channels;
    unsigned char* L = frame->data[0];
    unsigned char* R = frame->data[1];
    unsigned char* data = nullptr;
    if (channels == 1)
    {
        pushResampled(frame->data[0], frame->nb_samples * sample_size, frame->pts);
        return;
    }

    //暂时支持双通道
    switch (frame->format)
    {
        //case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
        case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
        case AV_SAMPLE_FMT_FLTP:        ///< float, planar
        {
            buf.resize(frame->linesize[0]);
            data = buf.data();
            // LLLL RRRR  32
            // LR LR LR LR 4
            for (int i = 0; i < frame->nb_samples; i++)
            {
                memcpy(data + i * sample_size * channels,
                    L + i * sample_size, sample_size);
                memcpy(data + i * sample_size * channels + sample_size,
                    R + i * sample_size, sample_size);
            }
            pushResampled(data, frame->linesize[0], frame->pts);
            return;
        }
        default:
            break;
    }
    pushResampled(frame->data[0], frame->linesize[0], frame->pts);
}

bool IDonutAudioPlayer::open(DonutAVParamWarpper& para)
{
    if (para.time_base->num > 0)
    {
        timebase_ = (double)para.time_base->den / (double)para.time_base->num;
        tb_ = *para.time_base;

        //一定要使用format context的timebase，否则对不上解码帧的时间
        //tb_ = para.samplerate_;
    }

    return open(para.para);
}

bool IDonutAudioPlayer::open(AVCodecParameters* para)
{
    //spec.channels = para->channels;
    input_spec_.channels = para->ch_layout.nb_channels;
    input_spec_.av_fmt = (AVSampleFormat)para->format;
    input_spec_.sample_rate = para->sample_rate;
    input_spec_.sample_size = av_get_bytes_per_sample((AVSampleFormat)para->format);

    switch (para->format)
    {
        case AV_SAMPLE_FMT_S16:         ///< signed 16 bits
        case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
            input_spec_.sdl_fmt = AUDIO_S16;
            break;
        case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
        case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
            input_spec_.sdl_fmt = AUDIO_S32;
            break;
        case AV_SAMPLE_FMT_FLT:         ///< float
        case AV_SAMPLE_FMT_FLTP:        ///< float, planar
            input_spec_.sdl_fmt = AUDIO_F32;
            break;
        default:
            break;
    }

    resample_spec_.channels = para->ch_layout.nb_channels;
    resample_spec_.av_fmt = AV_SAMPLE_FMT_S16;
    resample_spec_.sample_rate = para->sample_rate;
    resample_spec_.sample_size = av_get_bytes_per_sample((AVSampleFormat)resample_spec_.av_fmt);
    resample_spec_.sdl_fmt = AUDIO_S16SYS;

    //unsigned short samples = 1024;
    //int frame_size = 0;
    //int bytes_per_sec = 0;

    //input_spec_.sdl_fmt = AUDIO_S16;
    return open(resample_spec_);
}

void IDonutAudioPlayer::clear()
{
    close();
    setSpeed(playback_speed_);
}

void IDonutAudioPlayer::pushResampled(const unsigned char* data, int size, double pts, double duration)
{
    std::unique_lock<std::mutex> lock(mtx_);

    resampled_datas_.push_back(DonutAudioData());
    resampled_datas_.back().pts = pts;
    resampled_datas_.back().duration = duration;
    resampled_datas_.back().data.assign(data, data + size);
}

void IDonutAudioPlayer::pushResampled(const unsigned char* data, int size, long long pts)
{
    std::unique_lock<std::mutex> lock(mtx_);

    resampled_datas_.push_back(DonutAudioData());
    resampled_datas_.back().pts = pts;
    resampled_datas_.back().data.assign(data, data + size);
}

void IDonutAudioPlayer::pushAudio(const unsigned char* data, int size, long long pts)
{
    std::unique_lock<std::mutex> lock(mtx_);

    audio_datas_.push_back(DonutAudioData());
    audio_datas_.back().pts = pts;
    audio_datas_.back().data.assign(data, data + size);
}

void IDonutAudioPlayer::setSpeed(float s)
{
    playback_speed_ = s;
    auto spec = input_spec_;
    auto old_sample_rate = spec.sample_rate;
    spec.sample_rate *= s;
    open(spec);
    input_spec_.sample_rate = old_sample_rate;
}

IDonutAudioPlayer* IDonutAudioPlayer::getInstance()
{
    static DonutSDLAudioPlayer cx;
    return &cx;
}

void IDonutAudioPlayer::setClocks(std::shared_ptr<DonutAVClock>& a_clock, std::shared_ptr<DonutAVClock>& v_clock)
{
    std::lock_guard<std::mutex> lock(mtx_);
    audio_clock_ = a_clock;
    video_clock_ = v_clock;
}

void IDonutAudioPlayer::updateAuidoPts(double pts, int64_t pos, int serial, double time)
{
    audio_clock_->pts_ = pts;
    audio_clock_->last_updated_ = time;
    audio_clock_->pts_drift_ = audio_clock_->pts_ - time;
    audio_clock_->serial_ = serial;
}

void IDonutAudioPlayer::setFrameQueue(std::shared_ptr<DonutAVFrameQueue>& queue)
{
    std::lock_guard<std::mutex> lock(mtx_);
    this->audio_frame_queue_ = queue;
}

IDonutAudioPlayer::IDonutAudioPlayer()
{
    //SDL_Init(SDL_INIT_AUDIO);
}

}