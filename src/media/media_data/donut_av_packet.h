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

	DonutAVPacket(AVPacket* packet, bool force_copy = true);
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

	bool isFlushPacket();

public:
	static std::shared_ptr<DonutAVPacket> flush_packet_;
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
	void setSerial() { serial_++; }

	int getSize() { std::lock_guard<std::mutex> lock(mtx_);  return size_; }

	int packetQueuePut(std::shared_ptr<DonutAVPacket>& pkt);
	int packetQueuePutNullpacket(std::shared_ptr<DonutAVPacket> pkt, int stream_index);
	int pakcetQueueInit();
	void packetQueueFlush();
	void packetQueueDestroy();
	void packetQueueAbort();
	void packetQueueStart();
	void packetQueueSetStream(AVStream* stream) { this->stream_ = stream; }
	std::shared_ptr<DonutAVPacket> packetQueueGet(int block, int* serial);

	void packetQueueSetStreamIndex(int index);
	int packetQueueGetStreamIndex();

	bool packetQueueHasEnoughPackets();

	void packetQueueSetMasterFlush(bool flush);
	bool packetQueueNeedFlush() { return flush_and_sync_; }
protected:
	//int packetQueuePutPrivate(std::shared_ptr<DonutAVPacket> pkt);
private:
	int64_t duration_;
	int size_;
	int nb_packets_;
	int abort_request_;
	int serial_;

	int stream_index_ = -1;
	AVStream* stream_;

	bool is_master_ = false;
	bool flush_and_sync_ = false;


	std::mutex mtx_;
	std::condition_variable cond_;

	std::queue<std::shared_ptr<DonutAVPacket>> pkt_list_;

	friend class DonutAVFrame;
};

}
#endif