#ifndef DONUT_AV_VIDEO_DECODE_HANDLER_H
#define DONUT_AV_VIDEO_DECODE_HANDLER_H

#include "i_donut_av_base_handler.h"
#include "donut_av_video_decoder.h"

#include "donut_av_data_tools.h"
#include "donut_buffers.h"

namespace Donut
{
	class DONUT_API DonutAVVideoDecodeHandler : public IDonutAVBaseHandler
	{
	public:
		DonutAVVideoDecodeHandler();
		~DonutAVVideoDecodeHandler();
		int openDecoder(AVCodecParameters* param);
		int openDecoder(std::shared_ptr<DonutAVParamWarpper> param);
		void handle(void* data) override;

		AVFrame* getDecodedFrame();
	protected:
		void threadLoop() override;
	private:
		bool is_need_play_ = false;

		DonutAVVideoDecoder decoder_;
		DonutAVPacketDataList pkt_list_;

		AVFrame* decoded_frame_ = nullptr;

		std::unique_ptr<DonutAVDataCache<AVFrame*>> cache_;
	};

}

#endif