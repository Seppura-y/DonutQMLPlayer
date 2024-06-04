#include "donut_av_packet.h"

#include "log.h"

extern"C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace Donut {


DonutAVPacket::DonutAVPacket()
	: packet_(av_packet_alloc())
{

}

DonutAVPacket::~DonutAVPacket()
{
	av_packet_unref(packet_);
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
	pkt->setSerial(serial_);

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
	cond_.notify_all();
	std::unique_lock<std::mutex> lock(mtx_);
	if (pkt_list_.size() == 0) return;
	
	while (pkt_list_.size())
	{
		std::shared_ptr<DonutAVPacket> pkt = pkt_list_.front();
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

std::shared_ptr<DonutAVPacket> DonutAVPacketQueue::packetQueueGet(int block, int* serial)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (abort_request_)
	{
		return nullptr;
	}

	while (pkt_list_.empty())
	{
		if (block)
		{
			DN_CORE_WARN("DonutAVPacketQueue::packetQueueGet cond wait");
			cond_.wait(lock);
		}
		else
		{
			return nullptr;
		}
	}

	auto ret_pkt = (pkt_list_.front());
	if (ret_pkt)
	{
		pkt_list_.pop();
		nb_packets_--;
		size_ -= ret_pkt->getSize();
		duration_ -= ret_pkt->getDuration();
		//pkt = pkt1;
		if (serial)
		{
			*serial = ret_pkt->getSerial();
		}
	}
	else
	{
		//cond_.wait(lock);
		return nullptr;
	}
	
	return ret_pkt;
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