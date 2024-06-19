#include "donut_sdl_audio_player.h"

extern"C"
{
    #include <libavcodec/avcodec.h>
}

namespace Donut
{
    DonutSDLAudioPlayer::DonutSDLAudioPlayer()
    {
        SDL_Init(SDL_INIT_AUDIO);

        sound_touch_ = new soundtouch::SoundTouch();
    }

    DonutSDLAudioPlayer::~DonutSDLAudioPlayer()
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    void DonutSDLAudioPlayer::updateHandler(void* data)
    {
        if (data)
        {
            auto frame = static_cast<AVFrame*>(data);

            {
                //std::unique_lock<std::mutex> lock(mtx_);
                //av_frame_ref(decoded_frame_, frame);
                push(frame);
                //frame_updated_ = false;
                //lock.unlock();
            }

            av_frame_unref(frame);
        }
    }

    void DonutSDLAudioPlayer::threadLoop()
    {

    }

    void DonutSDLAudioPlayer::pause(bool is_pause)
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

    bool DonutSDLAudioPlayer::open(DonutAudioSpec& spec)
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

    void DonutSDLAudioPlayer::close()
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        std::unique_lock<std::mutex> lock(mtx_);
        audio_datas_.clear();
        cur_pts_ = 0; //当前播放位置
        last_ms_ = 0;  //上次的时间戳
        pause_begin_ = 0;//暂停开始时间戳
    }

    void DonutSDLAudioPlayer::callback(unsigned char* stream, int len)
    {
        SDL_memset(stream, 0, len);

        std::unique_lock<std::mutex> lock(mtx_);

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

            SDL_MixAudio(
                stream + mixed_size,
                buf.data.data() + buf.offset,
                size, volume_
            );

            need_size -= size;
            mixed_size += size;
            buf.offset += size;
            if (buf.offset >= buf.data.size())
            {
                audio_datas_.pop_front();
            }
        }
    }

    long long DonutSDLAudioPlayer::getCurrentPts()
    {
        double ms = 0;

        if (last_ms_ > 0)
        {
            ms = GetCurrentTimeMsec() - last_ms_;//距离上次写入缓冲的播放时间毫秒
        }

        //pts 毫秒换算pts的时间基数
        if (timebase_ > 0)
        {
            ms = ms / (double)1000 / (double)timebase_;
        }

        return cur_pts_ + speed_ * ms;
    }

}
