#include "donut_av_datas.h"

extern"C"
{
#include <libavcodec/avcodec.h>
}
#ifdef _WIN32
#include <Windows.h>
#include <sysinfoapi.h>
#endif

#include <chrono>
#include <thread>

namespace Donut
{
	int64_t GetCurrentTimeMsec()
	{
#ifdef _WIN32
		return (int64_t)GetTickCount();

#else
		return 0;
#endif
	}

	void SleepForMsec(int ms)
	{
		int64_t begin = (int64_t)GetTickCount();
		while (GetTickCount() - begin < ms)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	void FreeFrame(AVFrame** frame)
	{
		if (!frame)return;
		av_frame_free(frame);
	}

	DonutAVParamWarpper* DonutAVParamWarpper::create()
	{
		DonutAVParamWarpper* para = new DonutAVParamWarpper();
		return para;
	}
	DonutAVParamWarpper::~DonutAVParamWarpper()
	{
		if (para)
		{
			avcodec_parameters_free(&para);
		}
		if (time_base)
		{
			delete time_base;
			time_base = nullptr;
		}
	}

	DonutAVParamWarpper::DonutAVParamWarpper()
	{
		para = avcodec_parameters_alloc();
		time_base = new AVRational();
	}


	//void DonutAVPacketDataList::push(AVPacket* packet)
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	AVPacket* pkt = av_packet_alloc();
	//	av_packet_ref(pkt, packet);
	//	pkt_list_.push_back(pkt);
	//	if (pkt_list_.size() > max_list_)
	//	{
	//		if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
	//		{
	//			AVPacket* tmp = pkt_list_.front();
	//			av_packet_unref(tmp);
	//			av_packet_free(&tmp);
	//			pkt_list_.pop_front();
	//			//return;
	//		}
	//		while (!pkt_list_.empty())
	//		{
	//			if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
	//			{
	//				return;
	//			}
	//			AVPacket* tmp = pkt_list_.front();
	//			av_packet_unref(tmp);
	//			av_packet_free(&tmp);
	//			pkt_list_.pop_front();
	//		}
	//	}
	//}

	//AVPacket* DonutAVPacketDataList::pop()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	if (pkt_list_.empty())
	//	{
	//		return nullptr;
	//	}
	//	AVPacket* pkt = pkt_list_.front();
	//	pkt_list_.pop_front();
	//	return pkt;
	//}

	//void DonutAVPacketDataList::clear()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	while (!pkt_list_.empty())
	//	{
	//		AVPacket* tmp = pkt_list_.front();
	//		av_packet_unref(tmp);
	//		av_packet_free(&tmp);
	//		pkt_list_.pop_front();
	//	}
	//	return;
	//}



	//void DonutAVFrameDataList::push(AVFrame* frm)
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	if (frm_list_.size() > max_list_)
	//	{
	//		AVFrame* tmp = frm_list_.front();
	//		av_frame_unref(tmp);
	//		av_frame_free(&tmp);
	//		frm_list_.pop_front();
	//	}
	//	AVFrame* frame = av_frame_alloc();
	//	av_frame_ref(frame, frm);
	//	frm_list_.push_back(frame);
	//}

	//AVFrame* DonutAVFrameDataList::pop()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	if (frm_list_.empty())
	//	{
	//		return nullptr;
	//	}
	//	AVFrame* frame = frm_list_.front();
	//	frm_list_.pop_front();
	//	return frame;
	//}

	//void DonutAVFrameDataList::clear()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	if (!frm_list_.empty())
	//	{
	//		AVFrame* tmp = frm_list_.front();
	//		av_frame_unref(tmp);
	//		av_frame_free(&tmp);
	//		frm_list_.pop_front();
	//	}
	//}


	//void DonutAVCachedPacketDataList::push(AVPacket* packet)
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	AVPacket* pkt = av_packet_alloc();
	//	av_packet_ref(pkt, packet);
	//	pkt_list_.push_back(pkt);
	//	if (pkt_list_.size() > max_list_)
	//	{
	//		if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
	//		{
	//			AVPacket* tmp = pkt_list_.front();
	//			av_packet_unref(tmp);
	//			av_packet_free(&tmp);
	//			pkt_list_.pop_front();
	//			//return;
	//		}
	//		while (!pkt_list_.empty())
	//		{
	//			if (pkt_list_.front()->flags & AV_PKT_FLAG_KEY)
	//			{
	//				return;
	//			}
	//			AVPacket* tmp = pkt_list_.front();
	//			av_packet_unref(tmp);
	//			av_packet_free(&tmp);
	//			pkt_list_.pop_front();
	//		}
	//	}
	//}

	//AVPacket* DonutAVCachedPacketDataList::pop()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	if (pkt_list_.empty())
	//	{
	//		return nullptr;
	//	}
	//	AVPacket* pkt = pkt_list_.front();
	//	pkt_list_.pop_front();
	//	return pkt;
	//}

	//void DonutAVCachedPacketDataList::clear()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	while (!pkt_list_.empty())
	//	{
	//		AVPacket* tmp = pkt_list_.front();
	//		av_packet_unref(tmp);
	//		av_packet_free(&tmp);
	//		pkt_list_.pop_front();
	//	}
	//	return;
	//}

	//int DonutAVCachedPacketDataList::size()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	return pkt_list_.size();
	//}


}

