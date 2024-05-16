#ifndef DONUT_AV_VIDEO_DECODE_HANDLER_H
#define DONUT_AV_VIDEO_DECODE_HANDLER_H

#include "i_donut_av_base_handler.h"
#include "donut_av_video_decoder.h"

#include "donut_av_data_tools.h"
#include "donut_buffers.h"
#include "donut_av_stream.h"
#include "donut_av_frame.h"

namespace Donut
{
	class DONUT_API DonutAVDecodeHandler : public IDonutAVBaseHandler
	{
	public:
		DonutAVDecodeHandler();
		~DonutAVDecodeHandler();
		int openDecoder(AVCodecParameters* param);
		int openDecoder(std::shared_ptr<DonutAVParamWarpper> param);
		int openDecoder(std::shared_ptr<DonutAVStream> param);
		void updateHandler(void* data) override;

		template<typename T>
		void handle(std::shared_ptr<T> data);
		//virtual void handle(std::shared_ptr<DonutAVPacketQueue>& pkt_queue, std::shared_ptr<DonutAVFrameQueue>& frame_queue) override {}

		AVFrame* getDecodedFrame();

		void setFrameQueue(std::shared_ptr<DonutAVFrameQueue> frame_queue);
		void setPacketQueue(std::shared_ptr<DonutAVPacketQueue>& packet_queue);
	protected:
		void threadLoop() override;
	private:
		bool is_need_play_ = false;

		DonutAVVideoDecoder decoder_;
		DonutAVPacketDataList pkt_list_;

		AVFrame* decoded_frame_ = nullptr;

		std::unique_ptr<DonutAVDataCache<AVFrame*>> cache_;
		std::shared_ptr<DonutAVFrameQueue> frame_queue_;
		std::shared_ptr<DonutAVPacketQueue> packet_queue_;
	};

	template<typename T>
	inline void DonutAVDecodeHandler::handle(std::shared_ptr<T> data)
	{
	}

}

#endif