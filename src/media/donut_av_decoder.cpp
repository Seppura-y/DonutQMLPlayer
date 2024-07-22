#include "donut_av_decoder.h"

#include "log.h"
#include "donut_av_packet.h"
#include "donut_av_frame.h"

extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/avutil.h>
}

#include <iostream>


static void PrintError(int err)
{
	char buffer[1024] = { 0 };
	av_strerror(err, buffer, sizeof(buffer));
	DN_CORE_ERROR(buffer);
	//std::cout << buffer << std::endl;
}

#define PRINT_ERR_P(err) if(err != 0) {PrintError(err);return nullptr;}
#define PRINT_ERR_I(err) if(err != 0) {PrintError(err);return -1;}

#define GET_ERR_STR(err,str) 	char buffer[1024] = { 0 }; av_strerror(err, buffer, sizeof(buffer)); std::string str(buffer);

namespace Donut
{
	DonutAVDecoder::~DonutAVDecoder()
	{
		if (decoded_frame_)
		{
			//av_frame_unref(frame_);
			av_frame_free(&decoded_frame_);
		}
	}

	int DonutAVDecoder::openContext()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "OpenContext failed : codec_ctx_ is null" << std::endl;
			return -1;
		}
		int ret = -1;

		//AVDictionary* dict = nullptr;
		//av_dict_set(&dict, "tune", "zerolatency", 0);
		//av_dict_set(&dict, "preset", "ultrafast", 0);
		ret = avcodec_open2(codec_ctx_, nullptr, &options_);
		PRINT_ERR_I(ret);
		if (ret != 0)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_WARN(str);
		}

		if (codec_ctx_ && codec_ctx_->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (codec_ctx_->codec_id == AV_CODEC_ID_H264
				&& codec_ctx_->extradata
				&& codec_ctx_->extradata_size > 0)
			{
				int data_size = codec_ctx_->extradata_size;
				uint8_t* data = codec_ctx_->extradata;
				uint8_t* sps = nullptr;
				uint8_t* pps = nullptr;

				for (int i = 0; i < data_size; i++)
				{
					if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01)
					{
						if (sps == nullptr)
						{
							i += 4;
							sps = &data[i];
							continue;
						}
						if (sps && pps == nullptr)
						{
							i += 4;
							pps = &data[i];

							sps_size_ = pps - data - 8;
							pps_size_ = data_size - sps_size_ - 8;
							sps_data_.append((const char*)sps, sps_size_);
							pps_data_.append((const char*)pps, pps_size_);
							break;
						}
					}
				}
			}

			decoded_frame_ = av_frame_alloc();
			decoded_frame_->width = codec_ctx_->width;
			decoded_frame_->height = codec_ctx_->height;
			decoded_frame_->format = codec_ctx_->pix_fmt;
		}
		else if (codec_ctx_ && codec_ctx_->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			decoded_frame_ = av_frame_alloc();
			decoded_frame_->channels = codec_ctx_->channels;
			decoded_frame_->format = codec_ctx_->sample_fmt;
			
			decoded_frame_->channel_layout = av_get_default_channel_layout(decoded_frame_->channels);
			decoded_frame_->sample_rate = codec_ctx_->sample_rate;
			decoded_frame_->nb_samples = codec_ctx_->frame_size;
		}

		ret = av_frame_get_buffer(decoded_frame_, 0);
		//PRINT_ERR_I(ret);
		if (ret != 0)
		{
			GET_ERR_STR(ret, str);
			DN_CORE_WARN(str);
		}

		return 0;
	}

	int DonutAVDecoder::flushCodec()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_)
		{
			return avcodec_send_packet(codec_ctx_, nullptr);
		}
		return 0;
	}

	int DonutAVDecoder::sendPacket(AVPacket* pkt)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "Send Packet failed : codec_ctx_ is null" << std::endl;
			return -1;
		}

		auto is_null =  pkt->size == 0;

		int ret = avcodec_send_packet(codec_ctx_, pkt);
		av_packet_unref(pkt);

		if (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))
		{
			//std::cout << "avcodec_send_packet failed : ret == AVERROR(EAGAIN) || ret == AVERROR(EOF)" << std::endl;
			//PrintError(ret);

			GET_ERR_STR(ret,str);
			DN_CORE_WARN(str);

			return -1;
		}
		else if (ret >= 0)
		{
			return ret;
		}
		else
		{
			//PrintError(ret);
			GET_ERR_STR(ret, str);
			DN_CORE_WARN(is_audio_ ? "Audio Decoder " : "Video Decoder " + str + std::string(is_null ? " pkt null" : " pkt not null idx :"));
			return -1;
		}
	}

	int DonutAVDecoder::recvFrame(AVFrame* output_frame)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "Receive frame failed : codec_ctx_ is null" << std::endl;
			return -1;
		}
		int ret = 0;
		AVFrame* frame = output_frame;
		if(codec_ctx_->hw_device_ctx)
		{
			if ( !(frame = av_frame_alloc()) )
			{
				std::cout << "av_frame_alloc failed" << std::endl;
				ret = AVERROR(ENOMEM);
				return ret;
			}
		}

		ret = avcodec_receive_frame(codec_ctx_, frame);
		if (ret < 0)
		{
			if (ret == AVERROR(EAGAIN))
			{
				ret = -1;
			}
			else if (ret == AVERROR_EOF)
			{
				ret = -2;
			}
			else
			{
				//std::cout << "avcodec_receive_frame failed" << std::endl;
				//PRINT_ERR_I(ret);
				GET_ERR_STR(ret, str);
				DN_CORE_WARN(is_audio_ ? "Audio Decoder " : "Video Decoder " + str);
			}

			if (codec_ctx_->hw_device_ctx)
			{
				av_frame_free(&frame);
			}
			return ret;
		}

		if (codec_ctx_->hw_device_ctx)
		{
			ret = av_hwframe_transfer_data(output_frame, frame, 0);
			av_frame_free(&frame);
			if (ret != 0)
			{
				//std::cout << "av_hwframe_transfer_data failed" << std::endl;
				//PRINT_ERR_I(ret);

				GET_ERR_STR(ret, str);
				DN_CORE_WARN(is_audio_ ? "Audio Decoder " : "Video Decoder " + str);
				return ret;
			}
		}

		return 0;
	}

	int DonutAVDecoder::sendPacket(std::shared_ptr<DonutAVPacket> pkt)
	{
		int ret = sendPacket(pkt->getPacket());
		pkt.reset();
		return ret;
	}

	int DonutAVDecoder::recvFrame(std::shared_ptr<DonutAVFrame>& frame)
	{
		int ret = recvFrame(frame->getFrame());
		return ret;
	}

	int DonutAVDecoder::initHwDecode(int type)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (!codec_ctx_) return false;

		AVBufferRef* hw_device_ctx = nullptr;
		auto ret = av_hwdevice_ctx_create(&hw_device_ctx, (AVHWDeviceType)type, NULL, NULL, 0);
		if (ret != 0)
		{
			//PRINT_ERR_I(ret);
			GET_ERR_STR(ret, str);
			DN_CORE_WARN(is_audio_ ? "Audio Decoder " : "Video Decoder " + str);
			return ret;
		}
		codec_ctx_->hw_device_ctx = hw_device_ctx;
		std::cout << "hardware deivce : " << type << std::endl;
		return true;
	}

	uint8_t* DonutAVDecoder::getSpsData()
	{
		return nullptr;
	}

	uint8_t* DonutAVDecoder::getPpsData()
	{
		return nullptr;
	}

	int DonutAVDecoder::getSpsSize()
	{
		return 0;
	}

	int DonutAVDecoder::getPpsSize()
	{
		return 0;
	}

	void DonutAVDecoder::decoderFlush()
	{
		std::lock_guard<std::mutex> lock(mtx_);
		if (codec_ctx_)
		{
			avcodec_flush_buffers(codec_ctx_);
		}
	}

}