#ifndef DONUT_AV_PACKET_H
#define DONUT_AV_PACKET_H

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

struct AVPacket;

namespace Donut {

class DonutAVPacket
{
public:
	DonutAVPacket();
	~DonutAVPacket();

	DonutAVPacket(AVPacket* packet, bool force_copy);
	DonutAVPacket(const DonutAVPacket& other);
	DonutAVPacket(DonutAVPacket&& other);
	
	DonutAVPacket& operator=(const DonutAVPacket& other);

	void setSerial(int ser)
	{
		serial_ = ser;
	}

	int getSerial()
	{
		return serial_;
	}

	int getSize();
	int64_t getDuration();
	void setStreamIndex(int index);
private:
	AVPacket* packet_ = nullptr;
	int serial_;
};

class DonutAVPacketQueue
{
public:
	DonutAVPacketQueue();
	~DonutAVPacketQueue();
	int packetQueuePut(std::shared_ptr<DonutAVPacket> pkt);
	int packetQueuePutNullpacket(std::shared_ptr<DonutAVPacket> pkt, int stream_index);
	int pakcetQueueInit();
	void packetQueueFlush();
	void packetQueueDestroy();
	void packetQueueAbort();
	void packetQueueStart();
	int packetQueueGet(std::shared_ptr<DonutAVPacket>& pkt, int block, int* serial);
protected:
	//int packetQueuePutPrivate(std::shared_ptr<DonutAVPacket> pkt);
private:
	int nb_packets_;
	int size_;
	int64_t duration_;
	int abort_request_;
	int serial_;

	std::mutex mtx_;
	std::condition_variable cond_;

	std::queue<std::shared_ptr<DonutAVPacket>> pkt_list_;
};

}
#endif