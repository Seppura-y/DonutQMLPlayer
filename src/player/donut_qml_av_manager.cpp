#include "donut_qml_av_manager.h"

#include <QTime>
#include "log.h"

extern"C"
{
#include <libavformat/avformat.h>
}

namespace Donut
{
	DonutQMLAVManager* DonutQMLAVManager::s_instance_ = nullptr;

	DonutQMLAVManager::DonutQMLAVManager(QQuickItem* parent)
		: QQuickItem(parent)
	{
		if (s_instance_)
		{
			delete s_instance_;
		}

		s_instance_ = this;


		QObject::connect(this, &DonutQMLAVManager::sigMediaEOF, this, &DonutQMLAVManager::onMediaEOF);
		//initManager();
	}


	DonutQMLAVManager::~DonutQMLAVManager()
	{
		this->stop();
		// 终止各个线程，防止在点击右上角关闭按钮后导致程序崩溃
		resetManager();
	}

	DonutQMLAVManager* DonutQMLAVManager::getInstance()
	{
		if (!s_instance_)
		{
			s_instance_ = new DonutQMLAVManager();
		}

		return s_instance_;
	}

	//QObject* DonutQMLAVManager::singletonProvider(QQmlEngine* engine, QJSEngine* scriptEngine)
	//{
	//	Q_UNUSED(engine)
	//	Q_UNUSED(scriptEngine)
	//	return DonutQMLAVManager::getInstance();
	//}

	void DonutQMLAVManager::threadLoop()
	{
		while (!is_exit_)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			auto a_serial = a_packet_queue_->getSerial();

			if (v_packet_queue_->packetQueueNeedFlush())
			{
				v_packet_queue_->packetQueueFlush();
			}

			if (a_packet_queue_->packetQueueNeedFlush())
			{
				a_packet_queue_->packetQueueFlush();
			}

			emit sigUpdateTimePosSec(audio_clock_->pts_);

			//int a_remaining = a_frame_queue_->frameQueueNbRemaining();
			//if (a_remaining)
			//{
			//	auto f = a_frame_queue_->frameQueuePeek().get()->getFrame();
			//	notify((void*)(a_frame_queue_->frameQueuePeek().get()->getFrame()));
			//	a_frame_queue_->frameQueueNext();
			//}

		}
	}

	void DonutQMLAVManager::updateHandler(void* data)
	{
		int num = 0;
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
		//if (data)
		//{
		//	AVPacket* pkt = static_cast<AVPacket*>(data);

		//	if (pkt->stream_index == video_index_)
		//	{
		//		auto dn_pkt = std::make_shared<DonutAVPacket>(pkt);
		//		v_packet_queue_->packetQueuePut(dn_pkt);
		//	}
		//	else if (pkt->stream_index == audio_index_)
		//	{
		//		auto dn_pkt = std::make_shared<DonutAVPacket>(pkt);
		//		a_packet_queue_->packetQueuePut(dn_pkt);
		//	}

		//	av_packet_unref(pkt);
		//}
	}

	int DonutQMLAVManager::resetManager()
	{
		if (demux_handler_)
		{
			demux_handler_->stop();
			delete demux_handler_;
			demux_handler_ = nullptr;
		}


		{
			if (a_decode_handler_)
			{
				a_decode_handler_->stop();
				delete a_decode_handler_;
				a_decode_handler_ = nullptr;
			}

			if (a_packet_queue_)
			{
				a_packet_queue_->packetQueueDestroy();
				a_packet_queue_.reset();
			}

			//if (a_frame_queue_)
			//{
			//	a_frame_queue_->frameQueueDestroy();
			//	a_frame_queue_.reset();
			//}
		}

		{
			if (v_decode_handler_)
			{
				v_decode_handler_->stop();
				delete v_decode_handler_;
				v_decode_handler_ = nullptr;
			}

			if (v_packet_queue_)
			{
				v_packet_queue_->packetQueueDestroy();
				v_packet_queue_.reset();
			}

			//if (v_frame_queue_)
			//{
			//	v_frame_queue_->frameQueueDestroy();
			//	v_frame_queue_.reset();
			//}
		}

		{
			if (audio_player_)
			{
				audio_player_->close();
				audio_player_->stop();
				audio_player_ = nullptr;
			}
		}
		return 0;
	}

	int DonutQMLAVManager::initManager()
	{
		demux_handler_ = new DonutAVDemuxHandler();
		a_decode_handler_ = new DonutAVDecodeHandler();
		v_decode_handler_ = new DonutAVDecodeHandler();

		v_packet_queue_ = std::make_shared<DonutAVPacketQueue>();
		v_frame_queue_ = std::make_shared<DonutAVFrameQueue>(v_packet_queue_, 3, 1);
		v_clock_ = std::make_shared<DonutAVClock>();

		a_packet_queue_ = std::make_shared<DonutAVPacketQueue>();
		a_frame_queue_ = std::make_shared<DonutAVFrameQueue>(a_packet_queue_, 9, 1);
		a_clock_ = std::make_shared<DonutAVClock>();

		// video decoder 要放在audio decoder上面，否则avcodec_send_packet时会警告 invalid arguement
		//demux_handler_->addNode(v_decode_handler_);
		//demux_handler_->addNode(a_decode_handler_);

		audio_clock_ = std::make_shared<DonutAVClock>();
		video_clock_ = std::make_shared<DonutAVClock>();

		demux_handler_->addNode(this);

		audio_player_ = IDonutAudioPlayer::getInstance();

		demux_handler_->setVideoQueue(v_packet_queue_);
		demux_handler_->setAudioQueue(a_packet_queue_);
		if (video_view_)
		{
			video_view_->setFrameQueue(v_frame_queue_);

			v_decode_handler_->setClocks(audio_clock_, video_clock_);
			v_decode_handler_->addNode(video_view_);
			v_decode_handler_->setPacketQueue(v_packet_queue_);
			v_decode_handler_->setFrameQueue(v_frame_queue_);
		}

		if (audio_player_)
		{
			//audio_player_->setFrameQueue(a_frame_queue_);
			audio_player_->setClocks(audio_clock_, video_clock_);
			a_decode_handler_->setClocks(audio_clock_, audio_clock_);
			a_decode_handler_->addNode(audio_player_);
			a_decode_handler_->setPacketQueue(a_packet_queue_);
			a_decode_handler_->setFrameQueue(a_frame_queue_);
		}

		return 0;
	}

	int DonutQMLAVManager::initManager(int width, int height, DonutPixFormat fmt, void* win_id)
	{
		return 0;
	}

	int DonutQMLAVManager::initManager(int width, int height, int fmt, void* win_id)
	{
		return 0;
	}

	void DonutQMLAVManager::setMute()
	{
		if (audio_player_)
		{
			audio_player_->setVolume(0);
		}
	}

	void DonutQMLAVManager::onVideoViewInitialized(QObject* view)
	{
		video_view_ = dynamic_cast<IDonutVideoView*>(view);
		if (video_view_)
		{
			initManager();
			video_view_->updateHandler(nullptr);
		}
		else
		{
			qDebug() << "video_view_ is nullptr";
		}
	}

	void DonutQMLAVManager::setDemuxer(DonutAVDemuxHandler* demuxer)
	{
		demux_handler_ = demuxer;
	}

	void DonutQMLAVManager::setVideoDecoder(DonutAVDecodeHandler* decoder)
	{
		v_decode_handler_ = decoder;
	}

	void DonutQMLAVManager::setAudioDecoder(DonutAVDecodeHandler* decoder)
	{
		a_decode_handler_ = decoder;
	}

	void DonutQMLAVManager::setVideoView(IDonutVideoView* view)
	{
		video_view_ = view;
	}

	Q_INVOKABLE void DonutQMLAVManager::setPlaybackRate(float value)
	{
		playback_speed_ = value;
		if (audio_player_)
		{
			audio_player_->setPlaybackSpeed(playback_speed_);
		}
	}

	Q_INVOKABLE float DonutQMLAVManager::getPlaybackRate()
	{
		return playback_speed_;
	}

	int DonutQMLAVManager::onOpenMediaFile(QString path)
	{
		if (path.toStdString() != current_url_)
		{
			resetManager();
		}

		initManager();

		// 2.5.F 检测码流是否已经播放完
		demux_handler_->setEofCallback([this]()
		{
			emit sigMediaEOF();
		});

		if (demux_handler_->openAVSource(path.toStdString().c_str()) == 0)
		{

			if (demux_handler_->hasVideo())
			{
				auto audio_timebase = *demux_handler_->copyAudioParameters()->time_base;
				auto video_timebase = *demux_handler_->copyVideoParameters()->time_base;
				audio_clock_->timebase_ = audio_timebase;
				video_clock_->timebase_ = video_timebase;
				video_view_->setClocks(audio_clock_, video_clock_);
				video_index_ = demux_handler_->getVideoIndex();

				v_decode_handler_->setStreamIndex(video_index_);
				v_decode_handler_->setStream(demux_handler_->getVideoStream(0));
				v_decode_handler_->openDecoder(demux_handler_->copyVideoParameters());

				video_view_->setFrameQueue(v_frame_queue_);
			}

			if (demux_handler_->hasAudio())
			{
				audio_index_ = demux_handler_->getAudioIndex();
				a_decode_handler_->setStreamIndex(audio_index_);
				a_decode_handler_->setStream(demux_handler_->getAudioStream(0));
				a_decode_handler_->openDecoder(demux_handler_->copyAudioParameters());

				a_packet_queue_->packetQueueSetStreamIndex(audio_index_);
				//a_frame_queue_ = std::make_shared<DonutAVFrameQueue>(a_packet_queue_, 9, 0);

				audio_player_->setFrameQueue(a_frame_queue_);
				
				auto warpper = demux_handler_->copyAudioParameters();
				audio_player_->open(*warpper);
			}

			this->addNode(audio_player_);

			total_duration_ = demux_handler_->getTotalDuration();
			emit sigUpdateTotalDurationSec(total_duration_);

			demux_handler_->start();
			v_decode_handler_->start();
			a_decode_handler_->start();
			this->start();
			audio_player_->start();
			video_view_->start();
			return 0;
		}
		else
		{
			return -1;
		}
	}

	int DonutQMLAVManager::onMediaPlayStart()
	{
		std::lock_guard<std::mutex> lock(mtx_);

		return 0;
	}
	void DonutQMLAVManager::onMediaEOF()
	{
		//qDebug() << "DonutQMLAVManager::onMediaEOF()";

		// 2.5.F 检测码流是否已经播放完

		if (is_loop_)
		{

		}
	}

	void DonutQMLAVManager::destroyManager()
	{
	}


	void DonutQMLAVManager::setTargetUrl(std::string url)
	{
	}

	void DonutQMLAVManager::updateTimePos(qint64 value)
	{
	}

	void DonutQMLAVManager::updateTotalDuration(QTime value)
	{
	}

	void DonutQMLAVManager::updateSoundVolume(int value)
	{
	}

	void DonutQMLAVManager::mediaEndReached()
	{
	}

	void DonutQMLAVManager::setPlaying()
	{
	}

	void DonutQMLAVManager::setPause()
	{
	}

	void DonutQMLAVManager::setStop()
	{
	}

	void DonutQMLAVManager::setTimePos(double value)
	{
	}

	void DonutQMLAVManager::setSoundVolume(int value)
	{
		if (audio_player_)
			audio_player_->setVolume(value);
	}

	void DonutQMLAVManager::onSeekingTimePos(double value)
	{
		demux_handler_->seekByTimePos(value);
		serial_++;
	}

	void DonutQMLAVManager::onSeekForward()
	{
		serial_++;
	}

	void DonutQMLAVManager::onSeekBackward()
	{
		serial_++;
	}

	void DonutQMLAVManager::onPlayOrPause(bool status)
	{
	}

	void DonutQMLAVManager::onStop()
	{
	}

	void DonutQMLAVManager::onSetPlaybackRate(float rate)
	{
	}
}