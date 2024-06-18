#include "donut_av_codec_base.h"
#include "donut_av_data_tools.h"

#include <iostream>
extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/avutil.h>
}

static void printError(int err)
{
	char buffer[1024] = { 0 };
	av_strerror(err, buffer, sizeof(buffer));
	std::cout << buffer << std::endl;
}

#define PRINT_ERR_P(err) if(err != 0) {printError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {printError(err);return -1;}

namespace Donut
{
DonutAVCodecBase::DonutAVCodecBase()
{

}

DonutAVCodecBase::~DonutAVCodecBase()
{
	if (codec_ctx_)
	{
		avcodec_free_context(&codec_ctx_);
	}
	if (options_)
	{
		av_dict_free(&options_);
	}
}

AVCodecContext* DonutAVCodecBase::createContext(int codec_id, bool is_decode)
{
	const AVCodec* codec = nullptr;
	if (is_decode)
	{
		codec = avcodec_find_decoder((AVCodecID)codec_id);
		if (codec == nullptr)
		{
			std::cout << "find decoder failed" << std::endl;
			return nullptr;
		}
	}
	else
	{
		codec = avcodec_find_encoder((AVCodecID)codec_id);
		if (codec == nullptr)
		{
			std::cout << "find encoder failed" << std::endl;
			return nullptr;
		}
	}



	AVCodecContext* codec_context = avcodec_alloc_context3(codec);
	if (codec_context == nullptr)
	{
		std::cout << "alloc codec context failed" << std::endl;
	}

	return codec_context;
}

void DonutAVCodecBase::setCodecContext(AVCodecContext* codec_ctx)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (codec_ctx_)
	{
		avcodec_free_context(&codec_ctx_);
	}
	codec_ctx_ = codec_ctx;

	if (codec_ctx_->codec_type == AVMEDIA_TYPE_AUDIO)
	{
		is_audio_ = true;
	}
	else
	{
		is_audio_ = false;
	}

}

//int DonutAVCodecBase::openContext(bool is_decode)
//{
//	std::unique_lock<std::mutex> lock(mtx_);
//	if (codec_ctx_ == nullptr)
//	{
//		std::cout << "OpenContext failed : codec_ctx_ is null" << std::endl;
//		return -1;
//	}
//	int ret = -1;
//	if (!is_decode)
//	{
//		AVDictionary* dict = nullptr;
//		//av_dict_set(&dict, "tune", "zerolatency", 0);
//		//av_dict_set(&dict, "preset", "ultrafast", 0);
//		ret = avcodec_open2(codec_ctx_, NULL, &dict);
//	}
//	else
//	{
//		ret = avcodec_open2(codec_ctx_, NULL, NULL);
//	}
//	if (ret != 0)
//	{
//		std::cout << "avcodec_open2 failed" << std::endl;
//		return -1;
//	}
//	//if (codec_ctx_ 
//	//	&& codec_ctx_->codec_type == AVMEDIA_TYPE_VIDEO 
//	//	&& codec_ctx_->codec_id == AV_CODEC_ID_H264 
//	//	&& codec_ctx_->extradata && codec_ctx_->extradata_size > 0)
//	//{
//	//	int data_size = codec_ctx_->extradata_size;
//	//	uint8_t* data = codec_ctx_->extradata;
//	//	uint8_t num_of_sps;
//	//	uint8_t num_of_pps;
//	//	uint8_t* sps;
//	//	uint8_t* pps;
//	//	uint8_t sps_index = -1;
//	//	uint8_t pps_index = -1;
//	//	sps_size_ = 0;
//	//	pps_size_ = 0;
//	//	for (int i = 1; i < data_size; i++)
//	//	{
//	//		if (data[i] == 0x67 && data[i - 2] == 0x00)
//	//		{
//	//			num_of_sps = data[i - 3] & 0x1f;
//	//			uint8_t tmp_size = data[i - 4];
//	//			sps = &data[i];
//	//			sps_index = sps - data;
//	//			do
//	//			{
//	//				sps_data_.append((const char*)sps, tmp_size);
//	//				sps_size_ += tmp_size;
//	//				sps = sps + tmp_size;
//	//				tmp_size = *(sps + 2);
//	//			} while (--num_of_sps);
//
//	//			//num_of_pps = *(sps + 1);
//	//			break;
//	//		}
//	//	}
//	//	pps_index = sps_index + sps_size_ +1;
//	//	num_of_pps = data[pps_index];
//	//	uint8_t tmp_size = data[pps_index + 2];
//	//	pps = &data[pps_index + 3];
//	//	do
//	//	{
//	//		pps_data_.append((const char*)pps, tmp_size);
//	//		pps_size_ += tmp_size;
//	//		pps = pps + tmp_size;
//	//		tmp_size = *(pps + 2);
//	//	} while (--num_of_pps);
//	//}
//	
//	
//	//if (codec_ctx_
//	//	&& codec_ctx_->codec_type == AVMEDIA_TYPE_VIDEO
//	//	&& codec_ctx_->codec_id == AV_CODEC_ID_H264
//	//	&& codec_ctx_->extradata && codec_ctx_->extradata_size > 0)
//	//{
//	//	int data_size = codec_ctx_->extradata_size;
//	//	uint8_t* data = codec_ctx_->extradata;
//	//	uint8_t* sps;
//	//	uint8_t* pps;
//
//	//	for (int i = 4; i < data_size; i++)
//	//	{
//	//		if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01)
//	//		{
//	//			sps_size_ = i - 4;
//	//			pps_size_ = data_size - i - 4;
//	//			pps = &data[i + 4];
//	//			break;
//	//		}
//	//	}
//	//	sps = &data[4];
//	//	sps_data_.append((const char*)sps, sps_size_);
//	//	pps_data_.append((const char*)pps, pps_size_);
//	//}
//
//	if (codec_ctx_
//		&& codec_ctx_->codec_type == AVMEDIA_TYPE_VIDEO
//		&& codec_ctx_->codec_id == AV_CODEC_ID_H264
//		&& codec_ctx_->extradata && codec_ctx_->extradata_size > 0)
//	{
//		int data_size = codec_ctx_->extradata_size;
//		uint8_t* data = codec_ctx_->extradata;
//		uint8_t* sps = nullptr;
//		uint8_t* pps = nullptr;
//
//		for (int i = 0; i < data_size; i++)
//		{
//			if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01)
//			{
//				if (sps == nullptr)
//				{
//					i += 4;
//					sps = &data[i];
//					continue;
//				}
//				if (sps && pps == nullptr)
//				{
//					i += 4;
//					pps = &data[i];
//
//					//sps_size_ = pps - data - 8;
//					//pps_size_ = data_size - sps_size_ - 8;
//					//sps_data_.append((const char*)sps, sps_size_);
//					//pps_data_.append((const char*)pps, pps_size_);
//					break;
//
//				}
//			}
//		}
//	}
//	return 0;
//}


int DonutAVCodecBase::setOption(const char* key, const char* value)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (codec_ctx_ == nullptr)
	{
		std::cout << "SetOption failed : codec_ctx_ is null" << std::endl;
		return -1;
	}
	int ret = av_opt_set(codec_ctx_->priv_data, key, value, 0);
	PRINT_ERR_I(ret);

	return 0;
}


int DonutAVCodecBase::setOption(const char* key, const int value)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (codec_ctx_ == nullptr)
	{
		std::cout << "SetOption failed : codec_ctx_ is null " << std::endl;
		return -1;
	}

	int ret = av_opt_set_int(codec_ctx_->priv_data, key, value, 0);
	PRINT_ERR_I(ret);

	return 0;
}

int DonutAVCodecBase::setOption(const AVDictionary* dict)
{
	int ret = av_dict_copy(&options_, dict, NULL);
	PRINT_ERR_I(ret);
	return 0;
}

int DonutAVCodecBase::getCodecExtraData(uint8_t* buffer, int& size)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!codec_ctx_)
	{
		std::cout << "GetCodecExtraData failed : codec_ctx_ is null" << std::endl;
		return -1;
	}
	if (codec_ctx_->codec_id == AV_CODEC_ID_H264)
	{
		memcpy(buffer, codec_ctx_->extradata, codec_ctx_->extradata_size);
		size = codec_ctx_->extradata_size;
	}
	else
	{
		return -1;
	}
	return 0;
}


std::shared_ptr<DonutAVParamWarpper> DonutAVCodecBase::copyCodecParam()
{
	std::shared_ptr<DonutAVParamWarpper> param;
	std::unique_lock<std::mutex> lock(mtx_);
	if (!codec_ctx_)
	{
		std::cout << "copy codec param failed : codec_ctx_ is null" << std::endl;
		return param;
	}
	param.reset(DonutAVParamWarpper::create());
	std::shared_ptr<DonutAVParamWarpper> warp = std::make_shared<DonutAVParamWarpper>();
	*param->time_base = codec_ctx_->time_base;
	avcodec_parameters_from_context(param->para, codec_ctx_);
	return param;
}

std::shared_ptr<AVCodecParameters> DonutAVCodecBase::copyCodecParameter()
{
	return std::shared_ptr<AVCodecParameters>();
}

AVCodecContext* DonutAVCodecBase::get_codec_ctx()
{
	std::unique_lock<std::mutex> lock(mtx_);
	return codec_ctx_;
}

}
