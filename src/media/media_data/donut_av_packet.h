#ifndef DONUT_AV_PACKET_H
#define DONUT_AV_PACKET_H

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

struct AVPacket;
struct AVStream;

#define MIN_FRAMES 25

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

	int& getSerial()
	{
		return serial_;
	}

	int64_t getDuration();
	int64_t getPts();
	int getSize();
	int getStreamIndex();

	void setStreamIndex(int index);

	AVPacket* getPacket() { return packet_; }
private:
	AVPacket* packet_ = nullptr;
	int serial_;
};

class DonutAVPacketQueue
{
public:
	DonutAVPacketQueue();
	~DonutAVPacketQueue();

	int isAbortRequest() { return abort_request_; }
	int getSerial() { return serial_; }

	int packetQueuePut(std::shared_ptr<DonutAVPacket>& pkt);
	int packetQueuePutNullpacket(std::shared_ptr<DonutAVPacket> pkt, int stream_index);
	int pakcetQueueInit();
	void packetQueueFlush();
	void packetQueueDestroy();
	void packetQueueAbort();
	void packetQueueStart();
	int packetQueueGet(std::shared_ptr<DonutAVPacket>& pkt, int block, int* serial);

	void packetQueueSetStreamIndex(int index);
	int packetQueueGetStreamIndex(int index);

	bool packetQueueHasEnoughPackets();
protected:
	//int packetQueuePutPrivate(std::shared_ptr<DonutAVPacket> pkt);
private:
	int64_t duration_;
	int size_;
	int nb_packets_;
	int abort_request_;
	int serial_;

	int stream_index_;
	AVStream* stream_;


	std::mutex mtx_;
	std::condition_variable cond_;

	std::queue<std::shared_ptr<DonutAVPacket>> pkt_list_;
};

}
#endif