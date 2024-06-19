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
            //ȥ����ͣ���¼�
            if (pause_begin_ > 0)
                last_ms_ += (GetCurrentTimeMsec() - pause_begin_);
            SDL_PauseAudio(0);
        }

    }

    bool DonutSDLAudioPlayer::open(DonutAudioSpec& spec)
    {
        this->spec_ = spec;
        //�˳���һ����Ƶ
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
        //��ʼ����
        SDL_PauseAudio(0);
        return true;
    }

    void DonutSDLAudioPlayer::close()
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        std::unique_lock<std::mutex> lock(mtx_);
        audio_datas_.clear();
        cur_pts_ = 0; //��ǰ����λ��
        last_ms_ = 0;  //�ϴε�ʱ���
        pause_begin_ = 0;//��ͣ��ʼʱ���
    }

    void DonutSDLAudioPlayer::callback(unsigned char* stream, int len)
    {
        SDL_memset(stream, 0, len);

        std::unique_lock<std::mutex> lock(mtx_);

        if (audio_datas_.empty())return;

        auto buf = audio_datas_.front();
        // 1 buf ����stream����  offset��¼λ��
        // 2 buf С��stream ����  ƴ��
        int mixed_size = 0;     //�Ѿ�������ֽ���
        int need_size = len;    //��Ҫ������ֽ���

        cur_pts_ = buf.pts;     //��ǰ���ŵ�pts
        last_ms_ = GetCurrentTimeMsec();     //��ʱ��ʼ����

        while (mixed_size < len)
        {
            if (audio_datas_.empty())break;

            buf = audio_datas_.front();

            int size = buf.data.size() - buf.offset;//ʣ��δ���������
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
            ms = GetCurrentTimeMsec() - last_ms_;//�����ϴ�д�뻺��Ĳ���ʱ�����
        }

        //pts ���뻻��pts��ʱ�����
        if (timebase_ > 0)
        {
            ms = ms / (double)1000 / (double)timebase_;
        }

        return cur_pts_ + speed_ * ms;
    }

}
