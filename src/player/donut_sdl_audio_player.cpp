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
        int num = 0;
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (data)
        {
            auto frame = static_cast<AVFrame*>(data);

            AVRational tb = AVRational{ 1, frame->sample_rate };

            if (audio_frame_queue_)
            {
                //std::unique_lock<std::mutex> lock(mtx_);
                auto dn_frame = audio_frame_queue_->frameQueuePeekWritable();

                if (dn_frame)
                {
                    dn_frame->pts_ = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
                    dn_frame->pos_ = frame->pkt_pos;
                    dn_frame->serial_ = audio_frame_queue_->getSerial();
                    dn_frame->duration_ = av_q2d(AVRational{ frame->nb_samples, frame->sample_rate });

                    av_frame_move_ref(dn_frame->frame_, frame);
                    audio_frame_queue_->frameQueuePush(dn_frame);

                    int a = 0;
                }
            }

            //int resampled = resampler_.resampleAudio(frame, (void**)&resampled_buffer_);


            ////double clock = nb_resampled_ / output_spec_.sample_rate* output_spec_.channels* av_get_bytes_per_sample((AVSampleFormat)output_spec_.av_fmt);
            //double clock = frame->pts + (double)frame->nb_samples / frame->sample_rate;
            //double duration = frame->duration;

            //nb_storage_ += resampled * resample_spec_.channels * av_get_bytes_per_sample((AVSampleFormat)resample_spec_.av_fmt);

            ////std::this_thread::sleep_for(std::chrono::milliseconds((int)duration));
            //if (resampled > 0)
            //{
            //    pushResampled(resampled_buffer_, resampled, duration);
            //}

            //av_frame_unref(frame);
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

    bool DonutSDLAudioPlayer::open(AudioSpec& spec)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        //this->input_spec_ = spec;

        //�˳���һ����Ƶ
        SDL_QuitSubSystem(SDL_INIT_AUDIO);

        SDL_Init(SDL_INIT_AUDIO);

        SDL_AudioSpec wanted_spec;

        SDL_AudioSpec actual_spec;

        const char* env;
        static const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
        static const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
        int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

        // 4.8.A.1 ��ȡ���������е����������������ֵ���Ϣ
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

        // ����channel_layout��ȡnb_channels�����������wanted_nb_channels��ƥ��ʱ���˴���������
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

        wanted_spec.format = spec.sdl_fmt; // Ŀǰ����ʱ������AUDIO_S16SYS
        wanted_spec.silence = 0;

        wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
        wanted_spec.userdata = this;
        wanted_spec.callback = audioCallback;
        //wanted_spec.callback = audioCB;

        SDL_AudioDeviceID audio_dev;

        while (!(audio_dev = SDL_OpenAudioDevice(
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

        // SDL �ڲ�����Ƶ��������С
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
        resample_spec.sample_rate = spec.sample_rate;
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
        resampled_buffer_ = (uint8_t*)av_malloc(
            resample_spec.bytes_per_sec
        );

        st_source_buffer_ = static_cast<soundtouch::SAMPLETYPE*>(malloc(resample_spec.bytes_per_sec));

        st_resample_buffer_ = static_cast<soundtouch::SAMPLETYPE*>(malloc(resample_spec.bytes_per_sec));

        sound_touch_ = new soundtouch::SoundTouch();
        sound_touch_->setSampleRate(resample_spec.sample_rate);
        sound_touch_->setChannels(resample_spec.channels);
        //sound_touch_->setTempo(2);
        sound_touch_->setTempo(playback_speed_);
        sound_touch_->setPitch(pitch_);

        // �����ص�����ʼ����
        SDL_PauseAudioDevice(audio_dev, 0);

        nb_per_second_ = resample_spec.bytes_per_sec;
        is_exit_ = false;
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

        //std::unique_lock<std::mutex> lock(mtx_);

        audio_callback_time_ = av_gettime_relative();




        int mixed_size = 0;
        int need_size = len;

        int buffer_offset = 0;

        //cur_pts_ = buf.pts;     //��ǰ���ŵ�pts
        //last_ms_ = GetCurrentTimeMsec();     //��ʱ��ʼ����

        while (mixed_size < len)
        {
            //if (resampled_datas_.empty())
            //{
            //    break;
            //}

            //auto af = audio_frame_queue_->frameQueuePeekReadable();
            int a = audioDecodeFrame();

            //if (nb_storage_ < nb_per_second_)
            //{
            //    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //    continue;
            //}

            if (resampled_datas_.empty()) break;

            DonutAudioData buf = resampled_datas_.front();

            uint8_t* raw_data = buf.data.data();
            int raw_data_size = buf.data.size();

            for (int i = 0; i < raw_data_size / 2; i++)
            {
                st_source_buffer_[i] = (resampled_buffer_[i * 2] + ((resampled_buffer_[i * 2 + 1]) << 8));
            }

            sound_touch_->putSamples(st_source_buffer_, raw_data_size);

            //std::this_thread::sleep_for(std::chrono::milliseconds(10));

            int num = sound_touch_->receiveSamples(st_resample_buffer_, raw_data_size / 4);
            if (num == 0)
            {
                continue;
            }
            else
            {
                int size = need_size;
                if (num > need_size)
                {
                    size = need_size;
                }

                SDL_MixAudioFormat(
                    stream + mixed_size,
                    (uint8_t*)st_resample_buffer_ + buffer_offset,
                    AUDIO_S16SYS,
                    size,
                    volume_
                );

                //SDL_MixAudio(
                //    stream + mixed_size,
                //    (uint8_t*)resampled_buffer_ + buffer_offset,
                //    size,
                //    volume_
                //);

                need_size -= raw_data_size;
                mixed_size += raw_data_size;
                buffer_offset += raw_data_size;

                //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                //DN_CORE_INFO("mixed size : {}", mixed_size);
            }
            
            int duration = buf.pts;

            //int size = buf.data.size() - buf.offset;//ʣ��δ���������
            //if (size > need_size)
            //{
            //    size = need_size;
            //}

            //SDL_MixAudio(
            //    stream + mixed_size,
            //    buf.data.data() + buf.offset,
            //    size, volume_
            //);

            //need_size -= size;
            //mixed_size += size;
            //buf.offset += size;
            //if (buf.offset >= buf.data.size())
            //{
            //    audio_datas_.pop_front();
            //}
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
        int data_size, resampled_data_size;
        int64_t dec_channel_layout;
        int wanted_nb_samples;
        std::shared_ptr<DonutAVFrame> af;
        //std::unique_lock<std::mutex> lock(mtx_);
        do
        {
            if (audio_frame_queue_ && audio_frame_queue_->frameQueueNbRemaining() == 0)
            {
                if ((av_gettime_relative() - audio_callback_time_) > 10000000LL * audio_hw_buf_size_ / resample_spec_.bytes_per_sec / 2)
                {
                    return -1;
                }

                av_usleep(1000);
            }

            if (!(af = audio_frame_queue_->frameQueuePeekReadable()))
            {
                return -1;
            }
            audio_frame_queue_->frameQueueNext();

        } while (af->serial_ != audio_frame_queue_->getSerial());

        printf("af->frame_->format: %d\n", af->frame_->format);
        AVSampleFormat f = (AVSampleFormat)(af->frame_->format);
        printf("f: %d\n", f);

        int asdf = af->frame_->format;


        data_size = av_samples_get_buffer_size(
            af->frame_->linesize,
            af->frame_->channels,
            af->frame_->nb_samples,
            f,
            //(AVSampleFormat)(af->frame_->format),
            0);

        dec_channel_layout =
            (af->frame_->channel_layout && af->frame_->channels == av_get_channel_layout_nb_channels(af->frame_->channel_layout)) ?
            af->frame_->channel_layout : av_get_default_channel_layout(af->frame_->channels);

        wanted_nb_samples = af->frame_->nb_samples;

        resampler_.initResampler(input_spec_, resample_spec_);

        //lock.unlock();
        int resampled_size = resampler_.resampleAudio(af, (void**)&resampled_buffer_);

        int duration = af->frame_->duration;
        if (resampled_size > 0)
        {
            pushResampled(resampled_buffer_, resampled_size, duration);
        }

        return resampled_size;
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
}
