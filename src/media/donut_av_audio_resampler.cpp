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
        int ret = 0;
        swr_ctx_ = swr_alloc_set_opts(
            nullptr,                                        // SwrContext
            AV_CH_LAYOUT_STEREO,                            // out_ch_layout
            (AVSampleFormat)out.format,                     // out_sample_fmt
            out.sample_rate,                                // out_sample_rate
            av_get_default_channel_layout(out.channels),    // in_ch_layout
            (AVSampleFormat)in.format,                      // in_sample_fmt
            in.sample_rate,                                 // in_sample_rate
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
        if (ret <= 0)
        {
            ret = -1;
            DN_CORE_ERROR("swr_init failed");
            return ret;
        }

        buffer_ = (uint8_t*)av_malloc(in.sample_rate * in.channels * 2);

        return 0;
    }

    int DonutAVAudioResampler::resampleAudio(AVFrame* input_frame, void** pcm)
    {
        uint8_t* buffer = nullptr;

        int nb_resampled = swr_convert(
            swr_ctx_,
            &buffer_,
            input_frame->nb_samples,
            (const uint8_t**)input_frame->data,
            input_frame->nb_samples
        );

        int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        int swr_data_size = nb_resampled * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

        double now_time = input_frame->pts * av_q2d(timebase_);
        if(now_time < )
        return 0;
    }

}
