#include "donut_av_audio_resampler.h"

#include "log.h"

extern"C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
}

namespace Donut
{

    DonutAVAudioResampler::DonutAVAudioResampler()
    {
    }

    DonutAVAudioResampler::~DonutAVAudioResampler()
    {
    }

    int DonutAVAudioResampler::initResampler(AudioSpec in, AudioSpec out)
    {
        input_spec_ = in;
        output_spec_ = out;

        int ret = 0;
        int ch_layout = av_get_default_channel_layout(out.channels);
        swr_ctx_ = swr_alloc_set_opts(
            nullptr,                                        // SwrContext
            ch_layout,                                      // out_ch_layout        AV_CH_LAYOUT_STEREO
            (AVSampleFormat)output_spec_.av_fmt,                     // out_sample_fmt
            output_spec_.sample_rate,                                // out_sample_rate
            av_get_default_channel_layout(output_spec_.channels),    // in_ch_layout
            (AVSampleFormat)input_spec_.av_fmt,                      // in_sample_fmt
            input_spec_.sample_rate,                                 // in_sample_rate
            0,
            nullptr
        );

        if (!swr_ctx_)
        {
            ret = -1;
            DN_CORE_ERROR("swr_alloc_set_opts failed");
            return ret;
        }

        ret = swr_init(swr_ctx_);
        if (ret < 0)
        {
            ret = -1;
            DN_CORE_ERROR("swr_init failed");
            return ret;
        }



        int output_sample_count = av_rescale_rnd(
            swr_get_delay(swr_ctx_, input_spec_.sample_rate) + input_spec_.samples,
            out.sample_rate,
            input_spec_.sample_rate,
            AV_ROUND_UP
        );

        int buffer_size = av_samples_alloc(
            &buffer_,
            nullptr,
            2,
            output_sample_count,
            (AVSampleFormat)output_spec_.av_fmt,
            0
        );


        if (buffer_size < 0)
        {
            return -1;
        }

        //buffer_ = (uint8_t*)av_malloc(in.sample_rate * in.channels * in.sample_size);

        return 0;
    }

    int DonutAVAudioResampler::resampleAudio(AVFrame* input_frame, void** pcm)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        uint8_t* buffer = nullptr;

        int nb_resampled = swr_convert(
            swr_ctx_,
            &buffer_,
            input_frame->nb_samples,
            (const uint8_t**)input_frame->data,
            input_frame->nb_samples
        );

        if (nb_resampled <= 0)
        {
            return -1;
        }
        else
        {
            int out_size = av_samples_get_buffer_size(nullptr, 2, nb_resampled, (AVSampleFormat)output_spec_.av_fmt, 1);
            //for (int i = 0; i < out_size; i++)
            //{
            //    printf("%02x ", buffer_[i]);
            //    if ((i + 1) % 16 == 0) printf("\n");
            //}

            *pcm = buffer_;

            //int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            //int swr_data_size = nb_resampled * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            int out_channels = input_spec_.channels;
            int swr_data_size = nb_resampled * out_channels * av_get_bytes_per_sample((AVSampleFormat)output_spec_.av_fmt);

            //double now_time = input_frame->pts * av_q2d(timebase_);
            //if(now_time < )
            return out_size;
        }

        return -1;
    }

}
