#ifndef DONUT_AV_VIDEO_DECODER_H
#define DONUT_AV_VIDEO_DECODER_H

#include "core.h"

#include "donut_av_frame.h"
#include "donut_av_packet.h"
#include "donut_av_codec_base.h"
struct AVPacket;
struct AVFrame;

namespace Donut
{
	class DONUT_API DonutAVDecoder : public DonutAVCodecBase
	{
	public:
		DonutAVDecoder() {}
		~DonutAVDecoder();

		virtual int openContext() override;
		virtual int flushCodec() override;

		int sendPacket(AVPacket* pkt);
		int recvFrame(AVFrame* frame);

		int sendPacket(std::shared_ptr<DonutAVPacket> pkt);
		int recvFrame(std::shared_ptr<DonutAVFrame>& frame);

		AVFrame* getFrame() { return decoded_frame_; }

		int initHwDecode(int type = 4); // AV_HWDEVICE_TYPE_DXVA2

		uint8_t* getSpsData();
		uint8_t* getPpsData();
		int getSpsSize();
		int getPpsSize();

		void resetSerial(int serial) { finished_serial_ = serial; }

		void decoderFlush();
	protected:
		AVFrame* decoded_frame_ = nullptr;



	protected:
		int width_ = 0;
		int height_ = 0;
		int fps_ = 25;
		int bit_rate_ = 500 * 1024;


		int sps_size_ = -1;
		int pps_size_ = -1;
		std::string sps_data_;
		std::string pps_data_;

		int finished_serial_ = -1;
	};

}

#endif