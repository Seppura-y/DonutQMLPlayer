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

            int resampled = resampler_.resampleAudio(frame, (void**)&resampled_buffer_);


            //double clock = nb_resampled_ / output_spec_.sample_rate* output_spec_.channels* av_get_bytes_per_sample((AVSampleFormat)output_spec_.av_fmt);
            double clock = frame->pts + (double)frame->nb_samples / frame->sample_rate;
            double duration = frame->duration;

            nb_storage_ += resampled * output_spec_.channels * av_get_bytes_per_sample((AVSampleFormat)output_spec_.av_fmt);

            //std::this_thread::sleep_for(std::chrono::milliseconds((int)duration));
            if (resampled > 0)
            {
                pushResampled(resampled_buffer_, resampled, duration);
            }

            av_frame_unref(frame);
        }

    }

    // version 4
    //void DonutSDLAudioPlayer::updateHandler(void* data)
    //{
    //    int num = 0;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    //    if (data)
    //    {
    //        auto frame = static_cast<AVFrame*>(data);

    //        nb_resampled_ = resampler_.resampleAudio(frame, (void**)&resampled_buffer_);
    //        av_frame_unref(frame);

    //        if (nb_resampled_ > 0)
    //        {
    //            for (int i = 0; i < nb_resampled_ / 2; i++)
    //            {
    //                st_sample_buffer_[i] = (resampled_buffer_[i * 2] + ((resampled_buffer_[i * 2 + 1]) << 8));
    //            }

    //            //pushResampled(resampled_buffer_, nb_resampled_, 1);
    //            sound_touch_->putSamples(st_sample_buffer_, nb_resampled_);

    //            std::this_thread::sleep_for(std::chrono::milliseconds(1));

    //            num = sound_touch_->receiveSamples(st_resample_buffer_, nb_resampled_);
    //            int unp = sound_touch_->numUnprocessedSamples();
    //            int pnum = num - unp;
    //            if (num == 0)
    //            {
    //                av_frame_unref(frame);
    //                return;
    //                //break;
    //            }
    //            else
    //            {
    //                pushAudio((uint8_t*)st_resample_buffer_, num, 1);
    //            
    //            }
    //        }

    //        av_frame_unref(frame);

    //    }

    //}

    //// version 3 速度貌似正常，但是有杂音
    //void DonutSDLAudioPlayer::updateHandler(void* data)
    //{
    //    int num = 0;

    //    if (data)
    //    {
    //        auto frame = static_cast<AVFrame*>(data);

    //        is_finished_ = false;

    //        nb_resampled_ = resampler_.resampleAudio(frame, (void**)&resampled_buffer_);

    //        av_frame_unref(frame);

    //        if (nb_resampled_ > 0)
    //        {
    //            for (int i = 0; i < nb_resampled_ / 2; i++)
    //            {
    //                st_sample_buffer_[i] = (resampled_buffer_[i * 2] + ((resampled_buffer_[i * 2 + 1]) << 8));
    //            }

    //            sound_touch_->putSamples(st_sample_buffer_, nb_resampled_);

    //            std::this_thread::sleep_for(std::chrono::microseconds(1));

    //            num = sound_touch_->receiveSamples(st_resample_buffer_, nb_resampled_);
    //            if (num == 0)
    //            {
    //                return;
    //                //break;
    //            }
    //            else
    //            {
    //                push((uint8_t*)st_resample_buffer_, num, 1);
    //                
    //            }
    //        }
    //    }

    //}

    // version 2 速度貌似正常，但是有杂音
    //void DonutSDLAudioPlayer::updateHandler(void* data)
    //{
    //    int num = 0;
    //    
    //    if (data)
    //    {
    //        auto frame = static_cast<AVFrame*>(data);

    //        //while (1)
    //        //{
    //            if (is_finished_)
    //            {
    //                is_finished_ = false;

    //                nb_resampled_ = resampler_.resampleAudio(frame, (void**)&resampled_buffer_);
    //                int s = sizeof(resampled_buffer_);
    //                cur_pts_ = frame->pts * av_q2d(tb_);
    //                if (cur_pts_ < clock_)
    //                {
    //                    cur_pts_ = clock_;
    //                }
    //                clock_ = cur_pts_;

    //                if (nb_resampled_ > 0)
    //                {
    //                    for (int i = 0; i < nb_resampled_ / 2; i++)
    //                    {
    //                        st_sample_buffer_[i] = (resampled_buffer_[i * 2] + ((resampled_buffer_[i * 2 + 1]) << 8));
    //                    }

    //                    sound_touch_->putSamples(st_sample_buffer_, nb_resampled_);

    //                    num = sound_touch_->receiveSamples(st_resample_buffer_, nb_resampled_);
    //                    if (num == 0)
    //                    {
    //                        //break;
    //                    }
    //                    else
    //                    {
    //                        int a = 1;
    //                        int b = 2;
    //                        std::lock_guard<std::mutex> lock(mtx_);
    //                        is_finished_ = true;
    //                    }
    //                }
    //                else
    //                {
    //                    //sound_touch_->flush();
    //                    //break;
    //                }
    //            }

    //            if (num == 0)
    //            {
    //                std::lock_guard<std::mutex> lock(mtx_);
    //                is_finished_ = true;
    //            }
    //            else
    //            {
    //                if(resampled_buffer_ == nullptr)
    //                {
    //                    num = sound_touch_->receiveSamples(st_resample_buffer_, nb_resampled_);
    //                }

    //                if (num == 0)
    //                {
    //                    std::lock_guard<std::mutex> lock(mtx_);
    //                    is_finished_ = true;
    //                    //continue;
    //                }
    //                push((uint8_t*)st_resample_buffer_, num, 1);
    //                av_frame_unref(frame);
    //                //break;
    //            }

    //            {
    //                //push(frame);
    //            }

    //            av_frame_unref(frame);
    //        //}
    //    }

    //}


    // version 1 速度不正常，有杂音
    //void DonutSDLAudioPlayer::updateHandler(void* data)
    //{
    //    int num = 0;

    //    if (data)
    //    {
    //        auto frame = static_cast<AVFrame*>(data);

    //        if (is_finished_)
    //        {
    //            is_finished_ = false;

    //            nb_resampled_ = resampler_.resampleAudio(frame, (void**)&resampled_buffer_);
    //            int s = sizeof(resampled_buffer_);
    //            cur_pts_ = frame->pts * av_q2d(tb_);
    //            if (cur_pts_ < clock_)
    //            {
    //                cur_pts_ = clock_;
    //            }
    //            clock_ = cur_pts_;

    //            if (nb_resampled_ > 0)
    //            {
    //                for (int i = 0; i < nb_resampled_ / 2; i++)
    //                {
    //                    st_sample_buffer_[i] = (resampled_buffer_[i * 2] + ((resampled_buffer_[i * 2 + 1]) << 8));
    //                }

    //                sound_touch_->putSamples(st_sample_buffer_, nb_resampled_);

    //                num = sound_touch_->receiveSamples(st_resample_buffer_, nb_resampled_);
    //                if (num == 0)
    //                {

    //                }
    //            }
    //            //else
    //            //{
    //            //    sound_touch_->flush();
    //            //}
    //        }

    //        if (num == 0)
    //        {
    //            std::lock_guard<std::mutex> lock(mtx_);
    //            is_finished_ = true;
    //        }
    //        else
    //        {
    //            push((uint8_t*)st_resample_buffer_, num, 1);
    //        }

    //        {
    //            //push(frame);
    //        }

    //        av_frame_unref(frame);
    //    }
    //}

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

    bool DonutSDLAudioPlayer::open(AudioSpec& spec)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        //this->input_spec_ = spec;

        //退出上一次音频
        SDL_QuitSubSystem(SDL_INIT_AUDIO);

        SDL_AudioSpec sdl_spec;
        sdl_spec.freq = spec.sample_rate;
        sdl_spec.format = spec.sdl_fmt;
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

        AudioSpec output_spec;
        output_spec.channels = spec.channels;
        output_spec.av_fmt = spec.av_fmt;
        output_spec.sample_rate = spec.sample_rate;
        output_spec.sample_size = av_get_bytes_per_sample((AVSampleFormat)output_spec.av_fmt);

        // 输出参数暂时按照输入设置
        //output_spec_ = output_spec;
        int ret = resampler_.initResampler(this->input_spec_, this->output_spec_);
        if (ret != 0)
        {
            is_resampler_init_ = false;
            return false;
        }

        is_resampler_init_ = true;

        // sample rate * channels * sizeof(short)
        resampled_buffer_ = (uint8_t*)av_malloc(
            output_spec_.sample_rate
            * output_spec_.channels
            * output_spec_.sample_size
        );
        
        st_sample_buffer_ = static_cast<soundtouch::SAMPLETYPE*>(
            malloc(output_spec_.sample_rate * output_spec_.channels * output_spec_.sample_size)
        );
        
        st_resample_buffer_ = static_cast<soundtouch::SAMPLETYPE*>(
            malloc(output_spec_.sample_rate * output_spec_.channels * output_spec_.sample_size)
        );

        sound_touch_ = new soundtouch::SoundTouch();
        sound_touch_->setSampleRate(output_spec_.sample_rate);
        sound_touch_->setChannels(output_spec_.channels);
        //sound_touch_->setTempo(2);
        sound_touch_->setTempo(playback_speed_);
        sound_touch_->setPitch(pitch_);

        //开始播放
        SDL_PauseAudio(0);

        nb_per_second_ = output_spec_.sample_rate * output_spec_.channels * output_spec_.sample_size;
        is_exit_ = false;
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

        if (resampled_datas_.empty())return;

        DonutAudioData buf = resampled_datas_.front();


        int mixed_size = 0;
        int need_size = len;

        int buffer_offset = 0;

        //cur_pts_ = buf.pts;     //当前播放的pts
        //last_ms_ = GetCurrentTimeMsec();     //计时开始播放

        while (mixed_size < len)
        {
            if (resampled_datas_.empty())
            {
                break;
            }

            //if (nb_storage_ < nb_per_second_)
            //{
            //    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //    continue;
            //}

            buf = resampled_datas_.front();

            uint8_t* raw_data = buf.data.data();
            int raw_data_size = buf.data.size();

            for (int i = 0; i < raw_data_size / 2; i++)
            {
                st_sample_buffer_[i] = (resampled_buffer_[i * 2] + ((resampled_buffer_[i * 2 + 1]) << 8));
            }

            sound_touch_->putSamples(st_sample_buffer_, raw_data_size);

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

                SDL_MixAudio(
                    stream + mixed_size,
                    (uint8_t*)resampled_buffer_ + buffer_offset,
                    size,
                    volume_
                );

                need_size -= raw_data_size;
                mixed_size += raw_data_size;
                buffer_offset += raw_data_size;
            }
            
            int duration = buf.pts;
            std::this_thread::sleep_for(std::chrono::microseconds(duration  * 100));

            //int size = buf.data.size() - buf.offset;//剩余未处理的数据
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

    // version 1不变速
    //void DonutSDLAudioPlayer::callback(unsigned char* stream, int len)
    //{
    //    if (!stream)
    //    {
    //        int a = len;
    //    }
    //    SDL_memset(stream, 0, len);

    //    std::unique_lock<std::mutex> lock(mtx_);

    //    if (audio_datas_.empty())return;

    //    DonutAudioData buf = audio_datas_.front();
    //    // 1 buf 大于stream缓冲  offset记录位置
    //    // 2 buf 小于stream 缓冲  拼接
    //    int mixed_size = 0;     //已经处理的字节数
    //    int need_size = len;    //需要处理的字节数

    //    cur_pts_ = buf.pts;     //当前播放的pts
    //    last_ms_ = GetCurrentTimeMsec();     //计时开始播放

    //    while (mixed_size < len)
    //    {
    //        if (audio_datas_.empty())
    //        {
    //            int a = 133;
    //            break;
    //            //std::this_thread::sleep_for(std::chrono::microseconds(1));
    //            //continue;
    //        }

    //        buf = audio_datas_.front();

    //        auto d = buf.data.data();

    //        int size = buf.data.size() - buf.offset;//剩余未处理的数据
    //        if (size > need_size)
    //        {
    //            size = need_size;
    //        }

    //        SDL_MixAudio(
    //            stream + mixed_size,
    //            buf.data.data() + buf.offset,
    //            size, volume_
    //        );

    //        need_size -= size;
    //        mixed_size += size;
    //        buf.offset += size;
    //        if (buf.offset >= buf.data.size())
    //        {
    //            audio_datas_.pop_front();
    //        }
    //    }
    //}

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
