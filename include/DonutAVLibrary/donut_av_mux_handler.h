#ifndef DONUT_AV_MUX_HANDLER_H
#define DONUT_AV_MUX_HANDLER_H

#include "i_donut_av_base_handler.h"
#include "donut_av_muxer.h"


namespace Donut
{
class DONUT_API DonutAVMuxHandler : public IDonutAVBaseHandler
{
public:
	DonutAVMuxHandler();
	~DonutAVMuxHandler();

	int openAVMuxer(const char* url,
		AVCodecParameters* video_param,
		AVRational* video_timebase,
		AVCodecParameters* audio_param,
		AVRational* audio_timebase,
		DonutAVMuxType type = DonutAVMuxType::DONUT_AV_MUX_TYPE_FILE,
		uint8_t* extra_data = nullptr,
		int extra_data_size = 0
	);

	int resetMuxer();
	virtual void threadLoop() override;
	virtual void handle(void* package) override;

	std::shared_ptr<DonutAVParamWarpper> copyVideoParameters();
	std::shared_ptr<DonutAVParamWarpper> copyAudioParameters();

private:
	DonutAVMuxer muxer_;

	std::string url_;
	DonutAVMuxType mux_type_ = DonutAVMuxType::DONUT_AV_MUX_TYPE_FILE;

	int video_index_ = -1;
	int audio_index_ = -1;

	bool has_video_ = false;
	bool has_audio_ = false;

	DonutAVParamWarpper* audio_params_ = nullptr;
	DonutAVParamWarpper* video_params_ = nullptr;

	uint8_t* extradata_ = nullptr;
	int extradata_size_ = 0;

	DonutAVPacketDataList pkt_list_;

};

}


#endif