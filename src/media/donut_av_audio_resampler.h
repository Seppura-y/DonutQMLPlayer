#ifndef DONUT_AV_AUDIO_RESAMPLER_H
#define DONUT_AV_AUDIO_RESAMPLER_H

#include "donut_av_datas.h"
#include "donut_av_frame.h"
#include <mutex>

struct SwrContext;
struct AVFrame;

namespace Donut
{
	class DonutAVAudioResampler
	{
	public:
		DonutAVAudioResampler();
		~DonutAVAudioResampler();

		int initResampler(AudioSpec in, AudioSpec out);

		int resampleAudio(AVFrame* input_frame, void** pcm);
		int resampleAudio(std::shared_ptr<DonutAVFrame>& frame, void** out);
		int resampleAudio(void** in, void** out);
	private:
		SwrContext* swr_ctx_ = nullptr;
		AudioSpec input_spec_;
		AudioSpec output_spec_;

		AVFrame* resampled_frame_ = nullptr;
		uint8_t* buffer_ = nullptr;

		AVRational timebase_{ 0,0 };

		std::mutex mtx_;
	};
}
#endif