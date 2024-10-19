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

		void setClocks(std::shared_ptr<DonutAVClock>& master_clock, std::shared_ptr<DonutAVClock>& clock);

		void setSleepTime(bool need_sync, double time);

		void setVideoFramerate(int num, int den);

		void setPaused(bool pause);
	protected:
		void threadLoop() override;

	private:
		double getFrameDiffTime(AVFrame* frame);
		double getDelayTime(double diff);
	private:
		bool is_need_play_ = false;

		DonutAVDecoder decoder_;
		DonutAVPacketDataList pkt_list_;

		AVFrame* decoded_frame_ = nullptr;

		std::unique_ptr<DonutAVDataCache<AVFrame*>> cache_;
		std::shared_ptr<DonutAVFrameQueue> frame_queue_;
		std::shared_ptr<DonutAVPacketQueue> packet_queue_;

		int stream_index_ = -1;

		std::shared_ptr<DonutAVClock> master_clock_;
		std::shared_ptr<DonutAVClock> clock_;

		bool is_need_sync_ = false;
		int sleep_ms_ = 0;

		AVRational timebase_{ 0 };
		AVRational framerate_{ 0 };

		int last_serial_ = -1;
	};

	template<typename T>
	inline void DonutAVDecodeHandler::handle(std::shared_ptr<T> data)
	{
	}

}

#endif