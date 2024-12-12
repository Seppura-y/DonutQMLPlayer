//#ifndef DONUT_AV_VIDEO_CODEC_H
//#define DONUT_AV_VIDEO_CODEC_H
//
//#include "core.h"
//#include "donut_av_codec_base.h"
//
//
//namespace Donut
//{
//	class DONUT_API DonutAVVideoCodec : public DonutAVCodecBase
//	{
//	public:
//		DonutAVVideoCodec() {};
//		virtual ~DonutAVVideoCodec() {};
//		virtual int openContext() = 0;
//		virtual int flushCodec() = 0;
//
//		int setOption(const char* key, const char* value);
//		int setOption(const char* key, const int value);
//
//		uint8_t* getSpsData();
//		uint8_t* getPpsData();
//		int getSpsSize();
//		int getPpsSize();
//
//	protected:
//		int width_ = 0;
//		int height_ = 0;
//		int fps_ = 25;
//		int bit_rate_ = 500 * 1024;
//
//
//		int sps_size_ = -1;
//		int pps_size_ = -1;
//		std::string sps_data_;
//		std::string pps_data_;
//
//	};
//}
//
//
//#endif