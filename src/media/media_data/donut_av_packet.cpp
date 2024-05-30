#include "donut_av_packet.h"

extern"C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace Donut {


DonutAVPacket::DonutAVPacket()
	: packet_(av_packet_alloc())
{
	packet_->flags = AV_PKT_FLAG_KEY;
}

DonutAVPacket::~DonutAVPacket()
{
	av_packet_free(&packet_);
	packet_ = nullptr;
}

DonutAVPacket::DonutAVPacket(AVPacket* packet, bool force_copy)
	: DonutAVPacket()
{
	av_packet_ref(packet_, packet);
	if (packet_->buf && force_copy)
	{
		const auto offset = packet_->data - packet_->buf->data;
		av_buffer_make_writable(&packet_->buf);
		packet_->data = packet_->buf->data + offset;
	}
}

DonutAVPacket::DonutAVPacket(const DonutAVPacket& other)
	: DonutAVPacket()
{
	*this = other;
}

DonutAVPacket::DonutAVPacket(DonutAVPacket&& other)
	: DonutAVPacket()
{
	*this = std::move(other);
}

DonutAVPacket& DonutAVPacket::operator=(const DonutAVPacket& other)
{
	av_packet_ref(this->packet_, other.packet_);
	return *this;
}


int DonutAVPacket::getSize()
{
	return packet_->size;
}

int64_t DonutAVPacket::getDuration()
{
	return packet_->duration;
}

int64_t DonutAVPacket::getPts()
{
	if (packet_)
		return packet_->pts;
	else
		return 0;
}

int DonutAVPacket::getStreamIndex()
{
	if (packet_)
	{
		return packet_->stream_index;
	}
	else
	{
		return -1;
	}
}

void DonutAVPacket::setStreamIndex(int index)
{
	packet_->stream_index = index;
}





DonutAVPacketQueue::DonutAVPacketQueue()
	: serial_(0), nb_packets_(0), size_(0), duration_(0), abort_request_(0)
{
}

DonutAVPacketQueue::~DonutAVPacketQueue()
{
}

int DonutAVPacketQueue::packetQueuePut(std::shared_ptr<DonutAVPacket>& pkt)
{
	int ret = 0;
	//std::shared_ptr<DonutAVPacket> pkt1 = std::make_shared<DonutAVPacket>();
	//pkt1 = pkt;
	pkt->setSerial(serial_);

	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (abort_request_)
		{
			return -1;
		}

		pkt_list_.push(pkt);

		nb_packets_++;
		size_ += pkt->getSize();
		duration_ += pkt->getDuration();

		cond_.notify_one();
	}

	return ret;
}

int DonutAVPacketQueue::packetQueuePutNullpacket(std::shared_ptr<DonutAVPacket> pkt, int stream_index)
{
	pkt->setStreamIndex(stream_index);

	return packetQueuePut(pkt);
}

int DonutAVPacketQueue::pakcetQueueInit()
{

	return 0;
}

void DonutAVPacketQueue::packetQueueFlush()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (pkt_list_.size() == 0) return;
	
	while (std::shared_ptr<DonutAVPacket> pkt = pkt_list_.front())
	{
		pkt.reset();
		pkt_list_.pop();
	}

	nb_packets_ = 0;
	size_ = 0;
	duration_ = 0;
	serial_++;
}

void DonutAVPacketQueue::packetQueueDestroy()
{
	packetQueueFlush();
}

void DonutAVPacketQueue::packetQueueAbort()
{
	std::unique_lock<std::mutex> lock(mtx_);
	abort_request_ = 1;
	cond_.notify_all();
}

void DonutAVPacketQueue::packetQueueStart()
{
	std::unique_lock<std::mutex> lock(mtx_);
	abort_request_ = 0;
	serial_++;
}

int DonutAVPacketQueue::packetQueueGet(std::shared_ptr<DonutAVPacket>& pkt, int block, int* serial)
{
	int ret = 0;
	std::unique_lock<std::mutex> lock(mtx_);
	if (abort_request_)
	{
		ret = -1;
		return ret;
	}

	while (pkt_list_.empty())
	{
		if (block)
			cond_.wait(lock);
		else
			return 0;
	}

	std::shared_ptr<DonutAVPacket> pkt1 = pkt_list_.front();
	if (pkt1)
	{
		pkt_list_.pop();
		nb_packets_--;
		size_ -= pkt1->getSize();
		duration_ -= pkt1->getDuration();
		pkt = pkt1;
		if (serial)
		{
			*serial = pkt1->getSerial();
		}

		ret = 1;
	}
	else
	{
		//cond_.wait(lock);
		ret = 0;
	}
	
	return ret;
}

void DonutAVPacketQueue::packetQueueSetStreamIndex(int index)
{
	std::unique_lock<std::mutex> lock(mtx_);
	stream_index_ = index;
}

int DonutAVPacketQueue::packetQueueGetStreamIndex(int index)
{
	std::unique_lock<std::mutex> lock(mtx_);
	return stream_index_;
}

bool DonutAVPacketQueue::packetQueueHasEnoughPackets()
{
	return stream_index_ < 0 || 
		abort_request_ || 
		nb_packets_ > MIN_FRAMES && (!duration_ || av_q2d(stream_->time_base) * duration_ > 1.0);
}

}