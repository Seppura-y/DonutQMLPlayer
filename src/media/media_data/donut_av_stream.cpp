//#include "donut_av_stream.h"
//
//extern"C"
//{
//#include <libavformat/avformat.h>
//}
//
//namespace Donut
//{
//	DonutAVStream::~DonutAVStream()
//	{
//		if (para_)
//		{
//			avcodec_parameters_free(&para_);
//		}
//
//		if (timebase_)
//		{
//			delete timebase_;
//		}
//	}
//
//	DonutAVStream* DonutAVStream::createInstance()
//	{
//		return new DonutAVStream();
//	}
//
//	DonutAVStream::DonutAVStream()
//	{
//		para_ = avcodec_parameters_alloc();
//		timebase_ = new DonutAVRational();
//	}
//}