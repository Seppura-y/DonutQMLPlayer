#include "donut_sdl_audio_player.h"


#include "log.h"

extern"C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/time.h>
}

namespace Donut
{

    static void audioCb(void* userdata, unsigned char* stream, int len)
    {
        SDL_memset(stream, 0, len);

        auto ap = (DonutSDLAudioPlayer*)userdata;
        ap->callback(stream, len);

        //ap->audio_callback_time_ = av_gettime_relative();

        int mixed_size = 0;
        int need_size = len;

        int buffer_offset = 0;

        while (mixed_size < len)
        {
            int a = ap->audioDecodeFrame();
        }
    }


    DonutSDLAudioPlayer::DonutSDLAudioPlayer()
    {
        SDL_Init(SDL_INIT_AUDIO);
    }

    DonutSDLAudioPlayer::~DonutSDLAudioPlayer()
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    void DonutSDLAudioPlayer::updateHandler(void* data)
    {
        //if (data)
        //{
        //    auto frame = static_cast<AVFrame*>(data);
        //    if (frame->data == nullptr) return;
        //    AVRational tb = AVRational{ 1, frame->sample_rate };

        //    if (audio_frame_queue_)
        //    {
        //        auto dn_frame = audio_frame_queue_->frameQueuePeekWritable();

        //        if (dn_frame)
        //        {
        //            dn_frame->pts_ = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
        //            dn_frame->pos_ = frame->pkt_pos;
        //            dn_frame->serial_ = audio_frame_queue_->getSerial();
        //            dn_frame->duration_ = av_q2d(AVRational{ frame->nb_samples, frame->sample_rate });

        //            av_frame_move_ref(dn_frame->frame_, frame);
        //            audio_frame_queue_->frameQueuePush(dn_frame);

        //            int a = 0;
        //        }
        //    }
        //}

    }

    void DonutSDLAudioPlayer::threadLoop()
    {
        while (!is_exit_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (audio_frame_queue_)
            {
                auto ser = audio_frame_queue_->getSerial();
            }
        }
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

    bool DonutSDLAudioPlayer::open(AudioSpec& spec)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        is_resampler_init_ = false;
        //this->input_spec_ = spec;

        //退出上一次音频
        //SDL_PauseAudio(1);
        SDL_CloseAudio();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        SDL_Init(SDL_INIT_AUDIO);

        SDL_AudioSpec wanted_spec;

        SDL_AudioSpec actual_spec;

        const char* env;
        static const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
        static const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
        int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

        // 4.8.A.1 获取环境变量中的声道数和升到布局等信息
        env = SDL_getenv("SDL_AUDIO_CHANNELS");
        if (env)
        {
            spec.channels = atoi(env);
            spec.ch_layout = av_get_default_channel_layout(spec.channels);
        }

        if (!spec.ch_layout || spec.channels != av_get_channel_layout_nb_channels(spec.ch_layout))
        {
            spec.ch_layout = av_get_default_channel_layout(spec.channels);
            spec.ch_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
        }

        // 根据channel_layout获取nb_channels，当传入参数wanted_nb_channels不匹配时，此处会作修正
        int wanted_nb_channels = av_get_channel_layout_nb_channels(spec.ch_layout);
        int64_t wanted_channel_layout = spec.ch_layout;

        spec.channels = av_get_channel_layout_nb_channels(spec.ch_layout);
        wanted_spec.channels = spec.channels;
        wanted_spec.freq = spec.sample_rate;

        if (spec.sample_rate <= 0 || spec.channels <= 0)
        {
            DN_CORE_ERROR("Invalid sample rate or channel count!");
            return false;
        }

        while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
        {
            next_sample_rate_idx--;
        }

        wanted_spec.format = spec.sdl_fmt; // 目前传入时已填入AUDIO_S16SYS
        wanted_spec.silence = 0;

        wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
        wanted_spec.userdata = this;
        wanted_spec.callback = audioCallback;
        //wanted_spec.callback = audioCB;

        //SDL_AudioDeviceID audio_dev;

        while (!(audio_dev_ = SDL_OpenAudioDevice(
            NULL,
            0,
            &wanted_spec,
            &actual_spec,
            SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE))
        )
        {
            av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
                wanted_spec.channels, wanted_spec.freq, SDL_GetError());

            wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];

            if (!wanted_spec.channels)
            {
                wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
                wanted_spec.channels = wanted_nb_channels;
                if (!wanted_spec.freq)
                {
                    av_log(NULL, AV_LOG_ERROR,
                        "No more combinations to try, audio open failed\n");
                    return -1;
                }
            }
            wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
        }

        //wanted_spec.freq = next_sample_rates[next_sample_rate_idx];
        //wanted_spec.channels = wanted_nb_channels;

        if (actual_spec.format != AUDIO_S16SYS)
        {
            DN_CORE_ERROR("SDL advised audio format %d is not supported!");
            return false;
        }

        if (actual_spec.channels != wanted_spec.channels)
        {
            wanted_channel_layout = av_get_default_channel_layout(actual_spec.channels);
            if (!wanted_channel_layout)
            {
                DN_CORE_ERROR("SDL advised channel count %d is not supported!", actual_spec.channels);
                return false;
            }
        }

        // SDL 内部的音频缓冲区大小
        audio_hw_buf_size_ = actual_spec.size;

        //if (SDL_OpenAudio(&wanted_spec, nullptr) < 0)
        //{
        //    std::cerr << SDL_GetError() << std::endl;
        //    return false;
        //}

        AudioSpec resample_spec;
        resample_spec.channels = actual_spec.channels;
        resample_spec.ch_layout = wanted_channel_layout;
        resample_spec.av_fmt = AV_SAMPLE_FMT_S16;
        resample_spec.sample_rate = actual_spec.freq;
        resample_spec.sample_size = av_get_bytes_per_sample((AVSampleFormat)resample_spec.av_fmt);
        resample_spec.frame_size = av_samples_get_buffer_size(
            NULL,                           // linesize
            resample_spec.channels,         // nb_channels
            1,                              // nb_samples
            resample_spec.av_fmt,           // sample_fmt
            1                               // align
        );

        resample_spec.bytes_per_sec = av_samples_get_buffer_size(
            NULL,                           // linesize
            resample_spec.channels,         // nb_channels
            resample_spec.sample_rate,      // nb_samples
            resample_spec.av_fmt,           // sample_fmt
            1                               // align
        );

        this->resample_spec_ = resample_spec;
        int ret = resampler_.initResampler(this->input_spec_, this->resample_spec_);
        if (ret != 0)
        {
            is_resampler_init_ = false;
            return false;
        }

        is_resampler_init_ = true;

        // sample rate * channels * sizeof(short)
        //resampled_buffer_ = (uint8_t*)av_malloc(
        //    this->resample_spec_.bytes_per_sec
        //);

        st_source_buffer_ = static_cast<soundtouch::SAMPLETYPE*>(malloc(this->resample_spec_.bytes_per_sec));

        st_resample_buffer_ = static_cast<soundtouch::SAMPLETYPE*>(malloc(this->resample_spec_.bytes_per_sec));

        sound_touch_ = new soundtouch::SoundTouch();
        sound_touch_->setSampleRate(this->resample_spec_.sample_rate);
        sound_touch_->setChannels(this->resample_spec_.channels);
        //sound_touch_->setTempo(2);
        sound_touch_->setTempo(playback_speed_);
        sound_touch_->setPitch(pitch_);

        // 启动回调，开始播放
        SDL_PauseAudioDevice(audio_dev_, 0);

        nb_per_second_ = this->resample_spec_.bytes_per_sec;
        is_exit_ = false;
        return true;
    }

    void DonutSDLAudioPlayer::close()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        audio_datas_.clear();
        cur_pts_ = 0; //当前播放位置
        last_ms_ = 0;  //上次的时间戳
        pause_begin_ = 0;//暂停开始时间戳
        is_exit_ = true;
    }

    void DonutSDLAudioPlayer::callback(unsigned char* stream, int len)
    {
        SDL_memset(stream, 0, len);

        audio_callback_time_ = av_gettime_relative();

        int mixed_size = 0;
        int need_size = len;

        int buffer_offset = 0;

        int resampled_size = 0;

        int sample_size = resample_spec_.sample_size;
        int num_channels = resample_spec_.channels;

        while (mixed_size < len)
        {
            int resampled = audioDecodeFrame();
            if (resampled < 0)
            {
                DN_CORE_ERROR("resamped < 0", resampled);
            }
            //if (resampled == 0) break;

            if (resampled_datas_.empty()) break;

            auto resampled_data = resampled_datas_.back();
            resampled_datas_.pop_back();

            resampled_data.data.size();
            if (audio_clock_)
            {
                auto pts = resampled_data.pts;
                pts *= av_q2d(tb_);
                if (pts > 0)
                {
                    std::unique_lock<std::mutex> lock(mtx_);
                    audio_clock_->pts_ = pts;
                    //DN_CORE_ERROR("audio_clock_->pts_ = {}", pts);
                }

            }
            for (int i = 0; i < resampled / 2; i++)
            {
                st_source_buffer_[i] = (resampled_data.data[i * 2] | ((resampled_data.data[i * 2 + 1]) << 8));
            }

            sound_touch_->putSamples(st_source_buffer_, resampled / 4);

            int num_samples_needed = (len - mixed_size) / (sample_size * num_channels);
            int num_samples = sound_touch_->receiveSamples(st_resample_buffer_, num_samples_needed);

            if (num_samples == 0)
            {
                continue;
            }

            int bytes_received = num_samples * sample_size * num_channels;
            if (mixed_size + bytes_received > len)
            {
                bytes_received = len - mixed_size;
                num_samples = bytes_received / (sample_size * num_channels);
            }
            SDL_MixAudioFormat(
                stream + mixed_size,
                (uint8_t*)st_resample_buffer_,
                AUDIO_S16SYS,
                bytes_received,
                volume_
            );

            mixed_size += bytes_received;
        }

        //updateAuidoPts(af_end_time_ - (double)(2 * audio_hw_buf_size_ + audio_write_buf_size_) / resample_spec_.bytes_per_sec, 0, 0);
        //if (mixed_size < len)
        //{
        //    SDL_memset(stream + mixed_size, 0, len - mixed_size);
        //}
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

        return cur_pts_ + playback_speed_ * ms;
    }

    int DonutSDLAudioPlayer::getSoundTouchData(void** pcm_buffer)
    {
        while (!is_exit_)
        {

        }
        return 0;
    }

    int DonutSDLAudioPlayer::audioDecodeFrame()
    {
        int resampled_data_size;
        std::shared_ptr<DonutAVFrame> af;
        do
        {
            if (audio_frame_queue_ && audio_frame_queue_->frameQueueNbRemaining() == 0)
            {
                if ((av_gettime_relative() - audio_callback_time_) > 10000000LL * audio_hw_buf_size_ / resample_spec_.bytes_per_sec / 2)
                {
                    return -1;
                }
            }

            if (!(af = audio_frame_queue_->frameQueuePeekReadable()))
            {
                return -1;
            }

            audio_frame_queue_->frameQueueNext();

        } while (af->serial_ != audio_frame_queue_->getSerial());

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        resampled_data_size = resampler_.resampleAudio(af, (void**)&resampled_buffer_);

        if (resampled_data_size > 0)
        {
            pushResampled(resampled_buffer_, resampled_data_size, af->frame_->pts);
        }

        if (!isnan(af->pts_))
        {
            af_end_time_ = af->pts_ + (double)af->frame_->nb_samples / af->frame_->sample_rate;
        }
        else
        {
            af_end_time_ = NAN;
        }
        //audio_frame_queue_->frameQueueNext();


        return resampled_data_size;
    }

    bool DonutSDLAudioPlayer::isNormalPlaybackRate()
    {
        if (playback_speed_ > 0.99 && playback_speed_ < 1.01)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void DonutSDLAudioPlayer::setPlaybackSpeed(float speed)
    {
        playback_speed_ = speed;
        if(sound_touch_)
            sound_touch_->setTempo(playback_speed_);
    }
}
