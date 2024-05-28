#include "donut_qml_av_manager.h"

#include <QTime>

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

		initManager();
	}


	DonutQMLAVManager::~DonutQMLAVManager()
	{
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

	int DonutQMLAVManager::initManager()
	{
		demux_handler_ = new DonutAVDemuxHandler();
		a_decode_handler_ = new DonutAVDecodeHandler();
		v_decode_handler_ = new DonutAVDecodeHandler();

		v_packet_queue_ = std::make_shared<DonutAVPacketQueue>();
		v_frame_queue_ = std::make_shared<DonutAVFrameQueue>(v_packet_queue_, 3, 1);
		v_clock_ = std::make_shared<DonutAVClock>();

		a_packet_queue_ = std::make_shared<DonutAVPacketQueue>();
		a_frame_queue_ = std::make_shared<DonutAVFrameQueue>(a_packet_queue_, 3, 1);
		a_clock_ = std::make_shared<DonutAVClock>();

		demux_handler_->addNode(a_decode_handler_);
		demux_handler_->addNode(v_decode_handler_);

		if (video_view_)
		{
			v_decode_handler_->addNode(video_view_);
			v_decode_handler_->setPacketQueue(v_packet_queue_);
			v_decode_handler_->setFrameQueue(v_frame_queue_);
		}
		else
		{
			return -1;
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

	void DonutQMLAVManager::destroyManager()
	{
	}

	void DonutQMLAVManager::resetManager()
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
	}

	void DonutQMLAVManager::onSeekingTimePos(double value)
	{

	}

	void DonutQMLAVManager::onSeekForward()
	{
	}

	void DonutQMLAVManager::onSeekBackward()
	{
	}

	void DonutQMLAVManager::onSetSoundVolume(int value)
	{
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

	void DonutQMLAVManager::onVideoViewInitialized(QObject* view)
	{
		this->video_view_ = dynamic_cast<IDonutVideoView*>(view);
		if (this->video_view_)
		{
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

	void DonutQMLAVManager::onOpenMediaFile(QString path)
	{
		qDebug() << path;
		if (demux_handler_)
		{
			demux_handler_->openAVSource(path.toStdString().c_str());
			demux_handler_->start();
		}
	}
}