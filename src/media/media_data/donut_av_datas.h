#ifndef DONUT_AV_DATAS_H
#define DONUT_AV_DATAS_H

extern"C"
{
#include <libavutil/avutil.h>
}

#include <list>
#include <iostream>
#include <string>

#include <mutex>

#include "core.h"

struct AVFrame;
struct AVPacket;
struct AVCodecParameters;
struct AVRational;


#define MIN_LOG_LEVEL Log_Info
#define LOG(s,level) if(level >= Log_Info)\
std::cout<<"level_" << level<<":"<<"file : "<<__FILE__<<" "<<"line : "<<__LINE__<<": "\
<<s<<"\n"<<std::endl;

#define LOGINFO(s)     LOG(s,Log_Info)
#define LOGDEBUG(s)  LOG(s,Log_Debug)
#define LOGERROR(s)   LOG(s,Log_Error)
#define LOGFATAL(s)	LOG(s,Log_Fatal)

#define GET_AV_STRERR(ret,str) {\
	char buffer[1024] = {0};\
	av_strerror(ret,buffer,sizeof(buffer));\
	str = std::string(buffer);\
}

namespace Donut
{
	enum class DONUT_API PixFormat
	{
		PIX_FORMAT_YUV420P = 0,
		PIX_FORMAT_NV12 = 23,
		PIX_FORMAT_ARGB = 25,
		PIX_FORMAT_RGBA = 26,
		PIX_FORMAT_BGRA = 28
	};

	enum class DONUT_API RenderType
	{
		RENDER_TYPE_SDL = 0,
		RENDER_TYPE_OPENGL
	};

	enum class DONUT_API LogLevel
	{
		Log_Info,
		Log_Debug,
		Log_Error,
		Log_Fatal
	};


	int64_t GetCurrentTimeMsec();
	void SleepForMsec(int ms);
	void FreeFrame(AVFrame** frame);

	class DONUT_API DonutAVParamWarpper
	{
	public:
		AVCodecParameters* para = nullptr;
		AVRational* time_base = nullptr;
		static DonutAVParamWarpper* create();
		~DonutAVParamWarpper();
		//private:
		DonutAVParamWarpper();

		DonutAVParamWarpper(const DonutAVParamWarpper&) = delete;
		DonutAVParamWarpper operator=(const DonutAVParamWarpper&) = delete;
	};

	class DONUT_API DonutAVPacketDataList
	{
	public:

		void push(AVPacket* packet);
		AVPacket* pop();
		void clear();
	private:
		std::list<AVPacket*> pkt_list_;
		int max_list_ = 500;
		std::mutex mtx_;
	};

	class DONUT_API DonutAVFrameDataList
	{
	public:
		void push(AVFrame* frame);
		AVFrame* pop();
		void clear();

	private:
		std::list<AVFrame*> frm_list_;
		int max_list_ = 500;
		std::mutex mtx_;
	};


	class DONUT_API DonutAVCachedPacketDataList
	{
	public:

		void push(AVPacket* packet);
		AVPacket* pop();
		void clear();
		int size();
	private:
		std::list<AVPacket*> pkt_list_;
		int max_list_ = 500;
		std::mutex mtx_;
	};



}

#endif
