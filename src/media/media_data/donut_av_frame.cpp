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
		return frame_queue_[(rindex_ + rindex_shown_) % max_size_];
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekNext()
	{
		auto a = frame_queue_[(rindex_ + rindex_shown_ + 1) % max_size_];
		return frame_queue_[(rindex_ + rindex_shown_ + 1) % max_size_];
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekLast()
	{
		return frame_queue_[rindex_];
	}

	std::shared_ptr<DonutAVFrame> DonutAVFrameQueue::frameQueuePeekWritable()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (size_ >= max_size_ && !pkt_queue_->isAbortRequest())
		{
			cond_.wait(lock);
		}

		if (pkt_queue_->isAbortRequest())
		{
			return nullptr;
		}

		return frame_queue_[windex_];
	}

	std::shared_ptr<DonutAVFrame>& DonutAVFrameQueue::frameQueuePeekReadable()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (size_ - rindex_shown_ <= 0 && !pkt_queue_->isAbortRequest())
		{
			cond_.wait(lock);
		}

		if (pkt_queue_->isAbortRequest())
		{
			auto null_frame = std::make_shared<DonutAVFrame>();
			return null_frame;
		}

		return frame_queue_[(rindex_ + rindex_shown_) % max_size_];
	}

	// update write index
	void DonutAVFrameQueue::frameQueuePush(std::shared_ptr<DonutAVFrame> frame)
	{
		if (++windex_ == max_size_)
		{
			windex_ = 0;
		}
		std::unique_lock<std::mutex> lock(mtx_);
		size_++;
		this->frame_queue_.push_back(frame);
		cond_.notify_one();
	}

	void DonutAVFrameQueue::frameQueueNext()
	{
		if (keep_last_ && !rindex_shown_)
		{
			rindex_shown_ = 1;
			return;
		}

		frame_queue_[rindex_].reset();
		if (++rindex_ == max_size_)
		{
			rindex_ = 0;
		}

		std::unique_lock<std::mutex> lock(mtx_);
		size_--;
		cond_.notify_one();
	}

	int DonutAVFrameQueue::frameQueueNbRemaining()
	{
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
		for (i = 0; i < max_size_; i++)
		{
			auto frame = frame_queue_[i];
			frame.reset();
		}
	}
}