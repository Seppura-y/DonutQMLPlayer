//#ifndef DONUT_AV_VIDEO_ENCODER_H
//#define DONUT_AV_VIDEO_ENCODER_H
//
//#include "core.h"
//#include "donut_av_video_codec.h"
//
//struct AVFrame;
//struct AVPacket;
//
//namespace Donut
//{
//	class DONUT_API DonutAVVideoEncoder : public DonutAVVideoCodec
//	{
//	public:
//		DonutAVVideoEncoder(){}
//		virtual ~DonutAVVideoEncoder();
//
//		virtual int openContext() override;
//		virtual int flushCodec() override;
//
//		int sendFrame(AVFrame* frame);
//		int recvPacket(AVPacket* packet);
//
//		AVFrame* getFrame() { return frame_; }
//	protected:
//		AVFrame* frame_ = nullptr;
//		AVPacket* encoded_packet_ = nullptr;
//	};
//}
//
//
//#endif