#ifndef DONUT_AV_VIDEO_DECODE_HANDLER_H
#define DONUT_AV_VIDEO_DECODE_HANDLER_H

#include "i_donut_av_base_handler.h"
#include "donut_av_decoder.h"

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

		void setStreamIndex(int index) { this->stream_index_ = index; }
		void setStream(AVStream* stream);

		template<typename T>
		void handle(std::shared_ptr<T> data);
		//virtual void handle(std::shared_ptr<DonutAVPacketQueue>& pkt_queue, std::shared_ptr<DonutAVFrameQueue>& frame_queue) override {}

		AVFrame* getDecodedFrame();

		void setFrameQueue(std::shared_ptr<DonutAVFrameQueue> frame_queue);
		void setPacketQueue(std::shared_ptr<DonutAVPacketQueue> packet_queue);

		void setClocks(std::shared_ptr<DonutAVClock>& a_clock, std::shared_ptr<DonutAVClock>& v_clock);
	protected:
		void threadLoop() override;
	private:
		bool is_need_play_ = false;

		DonutAVDecoder decoder_;
		DonutAVPacketDataList pkt_list_;

		AVFrame* decoded_frame_ = nullptr;

		std::unique_ptr<DonutAVDataCache<AVFrame*>> cache_;
		std::shared_ptr<DonutAVFrameQueue> frame_queue_;
		std::shared_ptr<DonutAVPacketQueue> packet_queue_;

		int stream_index_ = -1;

		std::shared_ptr<DonutAVClock> audio_clock_;
		std::shared_ptr<DonutAVClock> video_clock_;
	};

	template<typename T>
	inline void DonutAVDecodeHandler::handle(std::shared_ptr<T> data)
	{
	}

}

#endif