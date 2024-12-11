#include "donut_qml_av_manager.h"

#include <QTime>
#include "log.h"
#include "playlist_model.h"

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

		startTimer(500);
	}


	DonutQMLAVManager::~DonutQMLAVManager()
	{
		this->stop();
		// 终止各个线程，防止在点击右上角关闭按钮后导致程序崩溃
		resetManager();
	}

	void DonutQMLAVManager::timerEvent(QTimerEvent* ev)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		emit sigUpdateTimePosSec(audio_clock_->pts_);
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


			//DN_CORE_ERROR("sigUpdateTimePosSec({})", audio_clock_->pts_);
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
	}

	int DonutQMLAVManager::resetManager()
	{
		DN_CORE_INFO("DonutQMLAVManager::resetManager()");
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

			if (a_frame_queue_)
			{
				a_frame_queue_->frameQueueDestroy();
				a_frame_queue_.reset();
			}
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

			if (v_frame_queue_)
			{
				v_frame_queue_->frameQueueDestroy();
				v_frame_queue_.reset();
				DN_CORE_INFO("DonutQMLAVManager::resetManager() v_frame_queue_->frameQueueDestroy();");
			}
		}

		{
			if (audio_player_)
			{
				audio_player_->close();
				audio_player_->stop();
				audio_player_ = nullptr;
				DN_CORE_INFO("DonutQMLAVManager::resetManager() audio_player_ reset");
			}
		}

		{
			if (video_view_)
			{
				//video_view_->reset();
				video_view_->stop();
				//video_view_ = nullptr;
				DN_CORE_INFO("DonutQMLAVManager::resetManager() video_view_ reset");
			}
		}

		//setPlayOrPause(true);
		//initManager();
		DN_CORE_INFO("DonutQMLAVManager::resetManager() Ended");
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
			video_view_->setManager(this);
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

		//setPlayOrPause(false);
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

	Q_INVOKABLE void DonutQMLAVManager::setPlayOrPause(bool pause)
	{
		is_paused_ = pause;

		video_view_->setPaused(is_paused_);
		demux_handler_->setPaused(is_paused_);
		a_decode_handler_->setPaused(is_paused_);
		v_decode_handler_->setPaused(is_paused_);
	}

	Q_INVOKABLE float DonutQMLAVManager::getPlaybackRate()
	{
		return playback_speed_;
	}

	Q_INVOKABLE void DonutQMLAVManager::setPlayMode(int value)
	{

		return;
	}

	Q_INVOKABLE void DonutQMLAVManager::changePlayMode()
	{
		playmode_ = static_cast<Playmode>((static_cast<int>(playmode_) + 1) % 4);

		switch (playmode_)
		{
		case Playmode::PLAYMODE_FILE_ONCE:
		{
			playmode_ = Playmode::PLAYMODE_FILE_ONCE;
			//btn_playmode_->setText(QChar(0xE256));
			//btn_playmode_->setToolTip(QString::fromLocal8Bit("file once"));
			break;
		}
		case Playmode::PLAYMODE_FILE_LOOP:
		{
			playmode_ = Playmode::PLAYMODE_FILE_LOOP;
			//btn_playmode_->setText(QChar(0xF366));
			//btn_playmode_->setToolTip(QString::fromLocal8Bit("file loop"));
			break;
		}
		case Playmode::PLAYMODE_LIST_ONCE:
		{
			playmode_ = Playmode::PLAYMODE_LIST_ONCE;
			//btn_playmode_->setText(QChar(0xE0B5));
			//btn_playmode_->setToolTip(QString::fromLocal8Bit("list once"));
			break;
		}
		case Playmode::PLAYMODE_LIST_LOOP:
		{
			playmode_ = Playmode::PLAYMODE_LIST_LOOP;
			//btn_playmode_->setText(QChar(0xF364));
			//btn_playmode_->setToolTip(QString::fromLocal8Bit("list loop"));
			break;
		}
		}
		//emit sigPlaymodeChanged(playmode_);
	}

	Q_INVOKABLE void DonutQMLAVManager::setRotationX(float value)
	{
		video_view_->setRotationX(value);
		return Q_INVOKABLE void();
	}

	Q_INVOKABLE void DonutQMLAVManager::setRotationY(float value)
	{
		video_view_->setRotationY(value);
		return Q_INVOKABLE void();
	}

	Q_INVOKABLE void DonutQMLAVManager::setRotationZ(float value)
	{
		video_view_->setRotationZ(value);
		return Q_INVOKABLE void();
	}

	int DonutQMLAVManager::onOpenMediaFile(QString path)
	{
		this->stop();

		if (path.toStdString() != current_url_)
		{
			resetManager();
		}

		initManager();

		// 2.5.F 检测码流是否已经播放完
		demux_handler_->setEofCallback([this]()
		{
			QList<QUrl> playlist = PlaylistModel::getInstance()->getPlayList();
			int playing_index = PlaylistModel::getInstance()->getPlayingIndex();
			switch (playmode_)
			{
				case Playmode::PLAYMODE_FILE_ONCE:
				{
					is_paused_ = true;
					demux_handler_->setPaused(is_paused_);
					emit sigMediaEOF();
					return;
				}
				case Playmode::PLAYMODE_FILE_LOOP:
				{
					onSeekingTimePos(0);
					break;
				}
				case Playmode::PLAYMODE_LIST_ONCE:
				{
					QList<QUrl>::iterator it = playlist.begin() + playing_index;
					if (++it == playlist.end())
					{
						emit sigMediaEOF();
						break;
					}
					else
					{

					}
				}
				case Playmode::PLAYMODE_LIST_LOOP:
				{
					QList<QUrl>::iterator it = playlist.begin() + playing_index;
					if (++it == playlist.end())
					{
						it = playlist.begin();
					}
					else
					{

					}

					if ((*it).isEmpty())
					{
						return;
					}

					auto name = (*it).toString();
					onOpenMediaFile(name);
					break;

					//QString cur_name = lb_item_name_->text();
					//auto it = playlist_.find(cur_name);
					//if (++it == playlist_.end())
					//{
					//	it = playlist_.begin();
					//}
					//if ((*it).isEmpty())
					//{
					//	return;
					//}
					//auto name = it.key();
					//auto url = it.value();
					//openning_filename_ = name;
					//render_widget_->openMediaFile(url);
					////emit sigPlayNextFile();
					//break;
				}
			}
		});

		if (demux_handler_->openAVSource(path.toStdString().c_str()) == 0)
		{

			if (demux_handler_->hasVideo())
			{
				auto framerate = demux_handler_->getVideoFramerate();
				auto audio_timebase = *demux_handler_->copyAudioParameters()->time_base;
				auto video_timebase = *demux_handler_->copyVideoParameters()->time_base;
				audio_clock_->timebase_ = audio_timebase;
				video_clock_->timebase_ = video_timebase;
				video_view_->setClocks(audio_clock_, video_clock_);
				video_index_ = demux_handler_->getVideoIndex();

				v_decode_handler_->setStreamIndex(video_index_);
				v_decode_handler_->setStream(demux_handler_->getVideoStream(0));
				v_decode_handler_->openDecoder(demux_handler_->copyVideoParameters());
				v_decode_handler_->setVideoFramerate(framerate.num_, framerate.den_);

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
				audio_player_->setVolume(sound_volume_);
			}

			this->addNode(audio_player_);

			total_duration_ = demux_handler_->getTotalDuration();
			emit sigUpdateTotalDurationSec(total_duration_);

			demux_handler_->start();
			v_decode_handler_->start();
			a_decode_handler_->start();

			this->start();
			audio_player_->start();
			video_view_->stop();
			video_view_->start();

			is_paused_ = false;
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

		if (!is_loop_)
		{
			DN_CORE_ERROR("eof");
			if (demux_handler_)
			{
				demux_handler_->setPaused(is_paused_);
			}

			if (a_decode_handler_)
			{
				a_decode_handler_->setPaused(is_paused_);
			}

			if (v_decode_handler_)
			{
				v_decode_handler_->setPaused(is_paused_);
			}

			if (video_view_)
			{
				video_view_->setPaused(is_paused_);
			}

			if (audio_player_)
			{
				audio_player_->pause(is_paused_);
			}
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
		sound_volume_ = value;
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