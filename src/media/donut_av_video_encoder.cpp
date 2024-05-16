#include "donut_av_video_encoder.h"
#include <iostream>

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
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
	DonutAVVideoEncoder::~DonutAVVideoEncoder()
	{
		if (frame_)
		{
			av_frame_free(&frame_);
			frame_ = nullptr;
		}
		if (encoded_packet_)
		{
			av_packet_free(&encoded_packet_);
			encoded_packet_ = nullptr;
		}
	}

	int DonutAVVideoEncoder::openContext()
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
		ret = avcodec_open2(codec_ctx_, NULL, &options_);
		PRINT_ERR_I(ret);

		frame_ = av_frame_alloc();
		frame_->width = codec_ctx_->width;
		frame_->height = codec_ctx_->height;
		frame_->format = codec_ctx_->pix_fmt;
		
		ret = av_frame_get_buffer(frame_, 0);
		PRINT_ERR_I(ret);


		encoded_packet_ = av_packet_alloc();

		return 0;
	}

	int DonutAVVideoEncoder::flushCodec()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_)
		{
			return avcodec_send_frame(codec_ctx_, nullptr);
		}
		return 0;
	}

	int DonutAVVideoEncoder::sendFrame(AVFrame* frame)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "codec_ctx_ is nullptr" << std::endl;
			return -1;
		}
		int ret = av_frame_make_writable(frame);
		if (ret != 0)
		{
			return -1;
		}

		ret = avcodec_send_frame(codec_ctx_, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))
		{
			std::cout << "avcodec_send_frame falied (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))" << std::endl;
			printError(ret);
			return -1;
		}
		else if (ret == 0)
		{
			return 0;
		}
		else
		{
			std::cout << "avcodec_send_frame falied" << std::endl;
			printError(ret);
			return -1;
		}
	}

	int DonutAVVideoEncoder::recvPacket(AVPacket* packet)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (codec_ctx_ == nullptr)
		{
			std::cout << "codec_ctx_ is nullptr" << std::endl;
			return -1;
		}

		int ret = avcodec_receive_packet(codec_ctx_, packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR(EOF))
		{
			return -1;
		}
		else if(ret == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
		return 0;
	}

}
