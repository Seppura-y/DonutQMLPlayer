#ifndef DONUT_AV_CODEC_BASE_H
#define DONUT_AV_CODEC_BASE_H


#include <mutex>

#include "core.h"
#include "donut_av_datas.h"

struct AVCodec;
struct AVCodecContext;
struct AVDictionary;
struct AVCodecParameters;

namespace Donut
{
	class DONUT_API DonutAVCodecBase
	{
	public:
		static AVCodecContext* createContext(int codec_id, bool is_decode);

		DonutAVCodecBase();
		virtual ~DonutAVCodecBase();

		void setCodecContext(AVCodecContext* codec_ctx);

		virtual int openContext() = 0;
		virtual int flushCodec() = 0;

		void setStreamIndex(int idx) { this->stream_index_ = idx; }

		int setOption(const char* key, const char* value);
		int setOption(const char* key, const int value);
		int setOption(const AVDictionary* dict);

		virtual AVCodecContext* get_codec_ctx();
		std::shared_ptr<DonutAVParamWarpper> copyCodecParam();
		std::shared_ptr<AVCodecParameters> copyCodecParameter();

		int getCodecExtraData(uint8_t* buffer, int& size);

	protected:
		std::mutex mtx_;
		AVCodecContext* codec_ctx_ = nullptr;
		AVDictionary* options_ = nullptr;

		bool is_audio_ = false;
		int stream_index_ = -1;
	private:

	};

}

#endif