#include "donut_av_frame.h"

extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

namespace Donut
{
	DonutAVFrame::DonutAVFrame() noexcept
	{
		frame_ = av_frame_alloc();
	}

	DonutAVFrame::~DonutAVFrame()
	{
		if (frame_)
		{
			//av_frame_unref(frame_);
			av_frame_free(&frame_);
			frame_ = nullptr;
		}
	}

	DonutAVFrame::DonutAVFrame(AVFrame* frame)
		: DonutAVFrame()
	{
		if (!frame)
		{
			return;
		}
		av_frame_ref(frame_, frame);
	}

	DonutAVFrame::DonutAVFrame(const DonutAVFrame& other) noexcept
		: DonutAVFrame()
	{
		av_frame_ref(this->frame_, other.frame_);
	}

	DonutAVFrame::DonutAVFrame(const DonutAVFrame&& other) noexcept
		: DonutAVFrame()
	{
		*this = std::move(other);
	}

	DonutAVFrame::DonutAVFrame(std::shared_ptr<DonutAVFrame>& other) noexcept
		: DonutAVFrame()
	{
		AVRational tb = AVRational{ 1, other->frame_->sample_rate };

		pts_ = (other->frame_->pts == AV_NOPTS_VALUE) ? NAN : other->frame_->pts * av_q2d(tb);
		pos_ = other->frame_->pkt_pos;
		serial_ = other->serial_;
		duration_ = av_q2d(AVRational{ other->frame_->nb_samples, other->frame_->sample_rate });

		av_frame_move_ref(this->frame_, other->frame_);
	}

	DonutAVFrame& DonutAVFrame::operator=(const DonutAVFrame& other)
	{
		av_frame_unref(this->frame_);
		av_frame_ref(this->frame_, other.frame_);
		return *this;
	}

	DonutAVFrame& DonutAVFrame::operator=(const DonutAVFrame&& other)
	{
		av_frame_unref(this->frame_);
		av_frame_move_ref(this->frame_, other.frame_);
		return *this;
	}

	int DonutAVFrame::getWidth()
	{
		width_ = 0;
		if (frame_)
		{
			width_ = frame_->width;
		}
		return width_;
	}

	int DonutAVFrame::getHeight()
	{
		height_ = 0;
		if (frame_)
		{
			height_ = frame_->height;
		}
		return height_;
	}

	int DonutAVFrame::getFormat()
	{
		format_ = -1;
		if (format_)
		{
			format_ = frame_->format;
		}
		return format_;
	}

	void DonutAVFrame::setFrame(AVFrame* frame, int serial)
	{
		if (frame->width != 0)
		{
			//duration_ = (frame_rate_)
		}
		else if (frame->sample_rate != 0)
		{
			AVRational tb = AVRational{ 1, frame->sample_rate };

			pts_ = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			pos_ = frame->pkt_pos;
			serial_ = serial;
			duration_ = av_q2d(AVRational{ frame->nb_samples, frame->sample_rate });
		}


		av_frame_move_ref(this->frame_, frame);
	}

	void DonutAVFrame::setFrame(AVFrame* frame, int serial, AVRational timebase)
	{
		if (frame->width != 0)
		{
			//duration_ = (frame_rate_)
		}
		else if (frame->sample_rate != 0)
		{
			pts_ = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(timebase);
			pos_ = frame->pkt_pos;
			serial_ = serial;
			//duration_ = av_q2d(AVRational{ frame->nb_samples, frame->sample_rate });
			duration_ = frame->duration;
		}


		av_frame_move_ref(this->frame_, frame);
	}





	DonutAVFrameQueue::DonutAVFrameQueue()
	{
	}

	DonutAVFrameQueue::~DonutAVFrameQueue()
	{
	}

	DonutAVFrameQueue::DonutAVFrameQueue(std::shared_ptr<DonutAVPacketQueue>& queue, int max, int kp_last)
	{
		pkt_queue_ = queue;
		max_size_ = max;
		keep_last_ = !!kp_last;

		stream_index_ = queue->packetQueueGetStreamIndex();

		for (int i = 0; i < max_size_; i++)
		{
			auto frame = std::make_shared<DonutAVFrame>();
			frame_queue_.push_back(frame);
		}

		//size_ = max_size_;
		rindex_ = 0;
		windex_ = 0;
		rindex_shown_ = 0;
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeek()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		auto a = frame_queue_[(rindex_ + rindex_shown_) % max_size_];
		return frame_queue_[(rindex_ + rindex_shown_) % max_size_];
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekNext()
	{
		std::unique_lock<std::mutex> lock(mtx_);

		auto a = frame_queue_[(rindex_ + rindex_shown_ + 1) % max_size_];
		return frame_queue_[(rindex_ + rindex_shown_ + 1) % max_size_];
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekLast()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		return frame_queue_[rindex_];
	}

	//std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekWritable()
	//{
	//	std::unique_lock<std::mutex> lock(mtx_);
	//	if (size_ >= max_size_ && !pkt_queue_->isAbortRequest())
	//	{
	//		cond_.wait(lock);
	//	}

	//	if (pkt_queue_->isAbortRequest())
	//	{
	//		return nullptr;
	//	}

	//	return frame_queue_[windex_];
	//}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekWritable()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (size_ >= max_size_ && !pkt_queue_->isAbortRequest())
		{
			cond_.wait(lock);
			//if (cond_.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout)
			//{
			//	// 处理超时情况
			//	return nullptr;
			//}
		}

		if (pkt_queue_->isAbortRequest())
		{
			return nullptr;
		}

		return frame_queue_[windex_];
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekReadable()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (size_ - rindex_shown_ <= 0 && !pkt_queue_->isAbortRequest())
		{
			cond_.wait(lock);
			//if (cond_.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout)
			//{
			//	// 处理超时情况
			//	//lock.unlock();
			//	return std::make_shared<DonutAVFrame>();
			//}
		}

		if (pkt_queue_->isAbortRequest())
		{
			auto null_frame = std::make_shared<DonutAVFrame>();
			return null_frame;
		}

		return frame_queue_[(rindex_ + rindex_shown_) % max_size_];
	}

	// update write index
	void DonutAVFrameQueue::frameQueuePush(std::shared_ptr<DonutAVFrame>& frame)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (++windex_ == max_size_)
		{
			windex_ = 0;
		}
		size_++;
		//this->frame_queue_.push_back(frame);
		//frame_queue_[windex_].reset();
		//frame_queue_[windex_] = frame;
		cond_.notify_one();
	}

	void DonutAVFrameQueue::frameQueueNext()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (keep_last_ && !rindex_shown_)
		{
			rindex_shown_ = 1;
			return;
		}

		av_frame_unref(frame_queue_[rindex_]->frame_);
		if (++rindex_ == max_size_)
		{
			rindex_ = 0;
		}

		size_--;
		cond_.notify_one();
	}

	int DonutAVFrameQueue::frameQueueNbRemaining()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		return size_ - rindex_shown_;
	}

	int64_t DonutAVFrameQueue::frameQueueLastPos()
	{
		auto frame = frame_queue_[rindex_];
		if (rindex_shown_ && frame->getSerial() == pkt_queue_->getSerial())
		{
			return frame->pos_;
		}
		else
		{
			return -1;
		}
	}

	void DonutAVFrameQueue::frameQueueDestroy()
	{
		int i;
		std::unique_lock<std::mutex> lock(mtx_);
		for (i = 0; i < max_size_; i++)
		{
			auto frame = frame_queue_[i];
			frame.reset();
		}
	}
	void DonutAVFrameQueue::frameQueueReset()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		rindex_shown_ = 0;
	}
}