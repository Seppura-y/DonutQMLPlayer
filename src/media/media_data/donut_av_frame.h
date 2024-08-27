#ifndef DONUT_AV_FRAME_H
#define DONUT_AV_FRAME_H

#include "donut_av_packet.h"
#include "donut_av_timer.h"

#include <memory>
#include <vector>

struct AVFrame;
namespace Donut {

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))


class DonutAVFrame
{
public:
	DonutAVFrame() noexcept;
	~DonutAVFrame();
	DonutAVFrame(AVFrame* frame);
	DonutAVFrame(const DonutAVFrame& other) noexcept;
	DonutAVFrame(const DonutAVFrame&& other) noexcept;
	DonutAVFrame(std::shared_ptr<DonutAVFrame>& other) noexcept;

	DonutAVFrame& operator=(const DonutAVFrame& other);
	DonutAVFrame& operator=(const DonutAVFrame&& other);

	int getWidth();
	int getHeight();
	int getFormat();

	int getSerial() { return serial_; }
	AVFrame* getFrame() { return frame_; }
	void setFrame(AVFrame* frame, int serial);
	void setFrame(AVFrame* frame, int serial, AVRational timebase);
public:
	AVFrame* frame_ = nullptr;

	int width_ = 0;
	int height_ = 0;
	int format_ = -1;
	int serial_ = 0;

	double pts_ = 0;
	double duration_ = 0;

	int64_t pos_ = 0;
	DonutAVRational sample_rate_;
	int uploaded_ = 0;
	int flip_v_ = 0;

	friend class DonutAVFrameQueue;
};

class DonutAVFrameQueue
{
public:
	DonutAVFrameQueue();
	~DonutAVFrameQueue();
	DonutAVFrameQueue(std::shared_ptr<DonutAVPacketQueue>& queue, int max, int kp_last);

	std::shared_ptr<DonutAVFrame> frameQueuePeek();
	std::shared_ptr<DonutAVFrame> frameQueuePeekNext();
	std::shared_ptr<DonutAVFrame> frameQueuePeekLast();
	std::shared_ptr<DonutAVFrame> frameQueuePeekWritable();
	std::shared_ptr<DonutAVFrame> frameQueuePeekReadable();

	void frameQueuePush(std::shared_ptr<DonutAVFrame>& frame);
	void frameQueueNext();

	int frameQueueNbRemaining();

	int64_t frameQueueLastPos();

	void frameQueueDestroy();

	void frameQueueReset();

	int getSerial() { return pkt_queue_->getSerial(); }

	void setFrameTimer(double time) { frame_timer_ = time; }
	double getFrameTimer() { return frame_timer_; }
private:
	std::mutex mtx_;
	std::condition_variable cond_;
	std::shared_ptr<DonutAVPacketQueue> pkt_queue_;
	std::vector<std::shared_ptr<DonutAVFrame>> frame_queue_;

	int rindex_ = 0;
	int windex_ = 0;
	int size_ = 0;
	int max_size_ = 0;
	int keep_last_ = 0;
	int rindex_shown_ = 0;

	int stream_index_ = -1;

	double frame_timer_ = -1;
};


}
#endif