//#ifndef DONUT_PACKET_BUFFER_H
//#define DONUT_PACKET_BUFFER_H
//
//#include "core.h"
//#include "donut_av_packet.h"
//
//
//#include <memory>
//#include <mutex>
//#include <list>
//
//struct AVPacket;
//namespace Donut
//{
//	class DONUT_API DonutPacketBuffer
//	{
//	public:
//		DonutPacketBuffer();
//		~DonutPacketBuffer();
//
//		void push(const AVPacket* packet);
//		void clear();
//		DonutAVPacket fetch();
//	private:
//		std::unique_ptr<std::mutex> mtx_;
//		std::list<DonutAVPacket> pkt_list_;
//
//		double remain_duration_ = 0.0;
//		int64_t remain_bytes_ = 0;
//		int pos_ = 0;
//		int max_size_ = 500;
//	};
//}
//#endif