#include "donut_av_audio_player.h"
#include <iostream>
#include <SDL2/SDL.h>

extern "C"
{
#include <libavformat/avformat.h>
}

namespace Donut {
class DonutSDLAudioPlayer :public DonutAVAudioPlayer
{
public:
    void pause(bool is_pause)
    {
        if (is_pause)
        {
            SDL_PauseAudio(1);
            pause_begin_ = GetCurrentTimeMsec();
        }
        else
        {
            //去掉暂停的事件
            if (pause_begin_ > 0)
                last_ms_ += (GetCurrentTimeMsec() - pause_begin_);
            SDL_PauseAudio(0);
        }

    }

    bool open(DonutAVAudioSpec& spec)
    {
        this->spec_ = spec;
        //退出上一次音频
        SDL_QuitSubSystem(SDL_INIT_AUDIO);

        SDL_AudioSpec sdl_spec;
        sdl_spec.freq = spec.sample_rate;
        sdl_spec.format = spec.format;
        sdl_spec.channels = spec.channels;
        sdl_spec.samples = spec.samples;
        sdl_spec.silence = 0;
        sdl_spec.userdata = this;
        sdl_spec.callback = audioCallback;
        if (SDL_OpenAudio(&sdl_spec, nullptr) < 0)
        {
            std::cerr << SDL_GetError() << std::endl;
            return false;
        }
        //开始播放
        SDL_PauseAudio(0);
        return true;
    }

    void close()
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        std::unique_lock<std::mutex> lock(mux_);
        audio_datas_.clear();
        cur_pts_ = 0; //当前播放位置
        last_ms_ = 0;  //上次的时间戳
        pause_begin_ = 0;//暂停开始时间戳
    }

    void callback(unsigned char* stream, int len)
    {
        SDL_memset(stream, 0, len);
        std::unique_lock<std::mutex> lock(mux_);
        if (audio_datas_.empty())return;
        auto buf = audio_datas_.front();
        // 1 buf 大于stream缓冲  offset记录位置
        // 2 buf 小于stream 缓冲  拼接
        int mixed_size = 0;     //已经处理的字节数
        int need_size = len;    //需要处理的字节数
        cur_pts_ = buf.pts;     //当前播放的pts
        last_ms_ = GetCurrentTimeMsec();     //计时开始播放

        while (mixed_size < len)
        {
            if (audio_datas_.empty())break;
            buf = audio_datas_.front();
            int size = buf.data.size() - buf.offset;//剩余未处理的数据
            if (size > need_size)
            {
                size = need_size;
            }
            SDL_MixAudio(stream + mixed_size,
                buf.data.data() + buf.offset,
                size, volume_);
            need_size -= size;
            mixed_size += size;
            buf.offset += size;
            if (buf.offset >= buf.data.size())
            {
                audio_datas_.pop_front();
            }
        }
    }

    long long getCurrentPts() override
    {
        double ms = 0;
        if (last_ms_ > 0)
            ms = GetCurrentTimeMsec() - last_ms_;//距离上次写入缓冲的播放时间毫秒
        //pts 毫秒换算pts的时间基数
        if (timebase_ > 0)
            ms = ms / (double)1000 / (double)timebase_;
        return cur_pts_ + speed_ * ms;
    }

private:
    long long cur_pts_ = 0; //当前播放位置
    long long last_ms_ = 0;  //上次的时间戳
    long long pause_begin_ = 0;//暂停开始时间戳
};

void DonutAVAudioPlayer::push(AVFrame* frame)
{
    if (!frame || !frame->data[0])return;
    std::vector<unsigned char> buf;
    int sample_size = av_get_bytes_per_sample((AVSampleFormat)frame->format);
    //int channels = frame->channels;

    int channels = frame->ch_layout.nb_channels;
    unsigned char* L = frame->data[0];
    unsigned char* R = frame->data[1];
    unsigned char* data = nullptr;
    if (channels == 1)
    {
        push(frame->data[0], frame->nb_samples * sample_size, frame->pts);
        return;
    }
    //暂时支持双通道
    switch (frame->format)
    {
        //case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
    case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
    case AV_SAMPLE_FMT_FLTP:        ///< float, planar
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
        push(data, frame->linesize[0], frame->pts);
        return;
        break;
    default:
        break;
    }
    push(frame->data[0], frame->linesize[0], frame->pts);
}

bool DonutAVAudioPlayer::open(DonutAVParamWarpper& para)
{
    if (para.time_base->num > 0)
        timebase_ = (double)para.time_base->den / (double)para.time_base->num;
    return open(para.para);
}

bool DonutAVAudioPlayer::open(AVCodecParameters* para)
{
    DonutAVAudioSpec spec;

    //spec.channels = para->channels;
    spec.channels = para->ch_layout.nb_channels;
    spec.sample_rate = para->sample_rate;

    AVSampleFormat;
    switch (para->format)
    {
    case AV_SAMPLE_FMT_S16:         ///< signed 16 bits
    case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
        spec.format = AUDIO_S16;
        break;
    case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
    case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
        spec.format = AUDIO_S32;
        break;
    case AV_SAMPLE_FMT_FLT:         ///< float
    case AV_SAMPLE_FMT_FLTP:        ///< float, planar
        spec.format = AUDIO_F32;
        break;
    default:
        break;
    }
    return open(spec);
}

DonutAVAudioPlayer* DonutAVAudioPlayer::getInstance()
{
    static DonutSDLAudioPlayer cx;
    return &cx;
}

DonutAVAudioPlayer::DonutAVAudioPlayer()
{
    SDL_Init(SDL_INIT_AUDIO);
}


}